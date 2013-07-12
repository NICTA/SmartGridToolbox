#include "BalancedPowerFlowNR.h"
#include "Output.h"

namespace SmartGridToolbox
{
   // KLUDGE : I'm not confident that the access to ublas underlying data structures won't change in some way.
   // Nice but slow is to copy all the data.
   // Othewise we might need to rething the use of ublas.
   SolverVars::SolverVars(const ublas::compressed_matrix<double> & a, ublas::vector<double> & b);
   {
      aNnz = J_.nnz();
      aRows = &a.index1_data()[0]; 
      cCols = &a.index2_data()[0]; 
      aVals = &a.value_data()[0]; 
      b = &v.data()[0]; 
   }

   void BalancedPowerFlowNR::addBus(NRBus * bus)
   {
      switch (bus->type)
      {
         case BusType::SL :
            SLBusses_.push_back(bus);
            break;
         case BusType::PQ :
            PQBusses_.push_back(bus);
            break;
         case BusType::PV :
            error("PV busses are not supported yet.");
            break;
      }
   }

   void BalancedPowerFlowNR::validate()
   {
      // Determine sizes:
      nSL_ = SLBusses_.size();
      nPQ_ = PQBusses_.size();
      assert(nSL_ == 1); // May change in future...
      assert(nPQ_ > 0); // May change in future...
      nBus_ = nSL_ + nPQ_;
      nVar_ = 2 * nPQ_;

      // Insert PQ and PV busses into list of all busses.
      // PQ busses at start, slack bus last.
      busses_ = BusVec(); // Clear contents...
      busses_.reserve(nBus_);
      busses_.insert(busses_.end(), PQBusses_.begin(), PQBusses_.end());
      busses_.insert(busses_.end(), SLBusses_.begin(), SLBusses_.end());

      // Set array ranges:
      // Note ublas::range goes from begin (included) to end (excluded).
      rPQ_ = ublas::range(0, nPQ_);
      rAll_ = ublas::range(0, nPQ_ + 1);
      iSL_ = nPQ_;
      rx0_ = ublas::range(0, nPQ_);
      rx1_ = ublas::range(nPQ_, 2 * nPQ_);

      // Size all arrays:
      PPQ_.resize(nPQ_, false);
      QPQ_.resize(nPQ_, false);
      Vr_.resize(nBus_, false);
      Vi_.resize(nBus_, false);
      Y_.resize(nBus_, nBus_, false);
      G_.resize(nBus_, nBus_, false);
      B_.resize(nBus_, nBus_, false);
      x_.resize(nVar_, false);
      f_.resize(nVar_, false);
      J_.resize(nVar_, nVar_, false);
      JConst_.resize(nVar_, nVar_, false);

      // Index all PQ busses:
      for (int i = 0; i < nPQ_; ++i)
      {
         PQBusses_[i]->idxPQ = i;
      }

      // Set the slack voltages:
      V0_ = SLBusses_[0]->V; // Array copy.

      // Set the PPQ_ and QPQ_ arrays of real and reactive power on each terminal:
      for (int i = 0; i < nPQ_; ++i)
      {
         PPQ_(i) = (busses_[i]->S).real();
         QPQ_(i) = (busses_[i]->S).imag();
      }

      // Build the bus admittance matrix:
      buildBusAdmit();

      // And set G_ and B_:
      G_ = real(Y_);
      B_ = imag(Y_);

      // Set the part of J that doesn't update at each iteration.
      JConst_(rx0_, rx0_) = G_;
      JConst_(rx0_, rx1_) = -B_;
      JConst_(rx1_, rx0_) = B_;
      JConst_(rx1_, rx1_) = G_;
      J_ = JConst_; // We only need to redo the elements that we mess with!

   }

   void BalancedPowerFlowNR::buildBusAdmit()
   {
      for (const NRBranch * const branch : branches_)
      {
         const NRBus * busi = branch->busi;
         const NRBus * busk = branch->busk;

         int ibus = busi->idxPQ;
         int kbus = busk->idxPQ;

         Y_(ibus, ibus) += branch->Y[0][0];
         Y_(kbus, kbus) += branch->Y[1][1];
         Y_(ibus, kbus) += branch->Y[0][1];
         Y_(kbus, ibus) += branch->Y[1][0];
      }
   }

   void BalancedPowerFlowNR::initx()
   {
      for (int i = 0; i < nPQ_; ++i)
      {
         const NRBus & bus = *busses_[i + 1];
         x_(i) = V0_.real();
         x_(i + nPQ_) = V0_.imag();
      }
   }

   void BalancedPowerFlowNR::updateBusV()
   {
      // Get voltages on all busses.
      // Done like this with a copy because eventually we'll include PV busses too.
      VRD(Vr_, rPQ_) = VRD(x_, rx0_);
      VRD(Vi_, rPQ_) = VRD(x_, rx0_);
      Vr_(iSL_) = V0_.real();
      Vi_(iSL_) = V0_.imag();
   }

   void BalancedPowerFlowNR::updateF()
   {
      using namespace ublas;

      VRD x0{x_, rx0_};
      VRD x1{x_, rx1_};

      updateBusV();

      ublas::vector<double> M2 = element_prod(Vr_, Vr_) + element_prod(Vi_, Vi_);

      matrix_range<compressed_matrix<double>> Grng{G_, rPQ_, rAll_};
      matrix_range<compressed_matrix<double>> Brng{G_, rPQ_, rAll_};

      ublas::vector<double> dr = element_div((-element_prod(PPQ_, x0) - element_prod(QPQ_, x1)), M2)
                               + prod(Grng, Vr_) - prod(Brng, Vi_);
      ublas::vector<double> di = element_div((-element_prod(PPQ_, x1) + element_prod(QPQ_, x0)), M2)
                               + prod(Grng, Vi_) + prod(Brng, Vr_);

      VRD(f_, rx0_) = dr;
      VRD(f_, rx1_) = di;
   }

   void BalancedPowerFlowNR::updateJ()
   {
      using namespace ublas;

      VRD x0{x_, rx0_};
      VRD x1{x_, rx1_};

      updateBusV();

      ublas::vector<double> M2PQ = element_prod(x0, x0) + element_prod(x1, x1);
      ublas::vector<double> M4PQ = element_prod(M2PQ, M2PQ);

      for (int i = 0; i < nPQ_; ++i)
      {
         J_(i, i) = Jconst_(i, i) +
            (-PPQ(i) / M2(i) + 2 * Vr_(i) * (PPQ(i) * Vr_(i) + QPQ(i) * Vi_(i)) / M4(i));
         J_(i, i + nPQ_) = Jconst_(i, i) +
            (-QPQ(i) / M2(i) + 2 * Vi_(i) * (PPQ(i) * Vr_(i) + QPQ(i) * Vi_(i)) / M4(i));
         J_(i + nPQ_, i) = Jconst_(i, i) +
            ( QPQ(i) / M2(i) + 2 * Vr_(i) * (PPQ(i) * Vi_(i) - QPQ(i) * Vr_(i)) / M4(i));
         J_(i + nPQ_, i + nPQ_) = Jconst_(i, i) +
            (-PPQ(i) / M2(i) + 2 * Vi_(i) * (PPQ(i) * Vi_(i) - QPQ(i) * Vr_(i)) / M4(i));
      }
   }

   void BalancedPowerFlowNR::solve()
   {

      const double tol = 1e-20;
      const int maxiter = 20;
      initx();
      for (int i = 0; i < maxiter; ++ i)
      {
         updateF();
         updateJ();

         SolverVars solverVars(J_, f_);

         // TODO : Factor this stuff into another function.
         permR = new int[nVars_];
         permC = new int[nVars_];

         // TODO : copied over from Gridlab-D. What's this all about?
         // Set up storage pointers - single element, but need to be malloced for some reason.
         A_LU.Store = (void *)malloc(sizeof(NCformat));
         B_LU.Store = (void *)malloc(sizeof(DNformat));

         // Populate these structures - A_LU matrix
         A_LU.Stype = SLU_NC;
         A_LU.Dtype = SLU_D;
         A_LU.Mtype = SLU_GE;
         A_LU.nrow = nVars_;
         A_LU.ncol = nVars_;

         // Populate these structures - B_LU matrix
         B_LU.Stype = SLU_DN;
         B_LU.Dtype = SLU_D;
         B_LU.Mtype = SLU_GE;
         B_LU.nrow = nVars_;
         B_LU.ncol = 1;
#ifndef MT
         // superLU sequential options
         set_default_options (&options);
#endif
      }
   }

   void SLUSolve(int m, int n, int nnz, double * a, int * asub, int * xa, int nrhs, double * rhs)
   {
      SuperMatrix A, L, U, B;
      int * perm_r; // row permutations from partial pivoting
      int * perm_c; // column permutation vector.
      int info, i, permc_spec;
#ifndef MT
      superlu_options_t options;
      SuperLUStat_t stat;
#endif

      // Create matrix A in the format expected by SuperLU.
      dCreate_CompCol_Matrix(&A, m, n, nnz, a, asub, xa, SLU_NC, SLU_D, SLU_GE);

      // Create right-hand side matrix B. */
      dCreate_Dense_Matrix(&B, m, nrhs, rhs, m, SLU_DN, SLU_D, SLU_GE);

      if ( !(perm_r = intMalloc(m)) ) ABORT("Malloc fails for perm_r[].");
      if ( !(perm_c = intMalloc(n)) ) ABORT("Malloc fails for perm_c[].");
      // Set the default input options.
      set_default_options(&options);
      options.ColPerm = NATURAL;
      // Initialize the statistics variables.
      StatInit(&stat);
      // Solve the linear system.
      dgssv(&options, &A, perm_c, perm_r, &L, &U, &B, &stat, &info);
      dPrint_CompCol_Matrix("A", &A);
      dPrint_CompCol_Matrix("U", &U);
      dPrint_SuperNode_Matrix("L", &L);
      print_int_vec("\nperm_r", m, perm_r);
   }

/* De-allocate storage */
SUPERLU_FREE (rhs);
SUPERLU_FREE (perm_r);
SUPERLU_FREE (perm_c);
Destroy_CompCol_Matrix(&A);
Destroy_SuperMatrix_Store(&B);
Destroy_SuperNode_Matrix(&L);
Destroy_CompCol_Matrix(&U);
StatFree(&stat);
   }
}
