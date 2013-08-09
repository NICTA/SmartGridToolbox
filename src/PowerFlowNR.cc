#include <algorithm>
#include <ostream>
#include <sstream>
#include "PowerFlowNR.h"
#include "SparseSolver.h"

namespace SmartGridToolbox
{
   BusNR::BusNR(const std::string & id, BusType type, const std::vector<Phase> & phases, const Vector<Complex> & V,
         const Vector<Complex> & Y, const Vector<Complex> & I, const Vector<Complex> & S)
      id_(id),
      type_(type),
      idx_(-1)
   {
      int nPhase = phases.size();
      assert(V.size() == nPhase);
      assert(Y.size() == nPhase);
      assert(I.size() == nPhase);
      assert(S.size() == nPhase);
      for (int i = 0; i < nPhase; ++i)
      {
         nodes_.emplace_back(*this, phases[i], V(i), Y(i), I(i), S(i));
      }
   }

   NodeNR::NodeNR(Bus & bus, Phase phase, const Complex & V, const Complex & Y, const Complex & I, const Complex & S) :
      bus_(bus),
      phase_(phase),
      V_(V),
      Y_(Y),
      I_(I),
      S_(S),
      idx_(-1)
   {
      // Empty. 
   }
   
   BranchNR::BranchNR(const std::string & id, const std::string & idBus0, const std::string & idBus1, 
         const std::vector<Phase> & phasesBus0, const std::vector<Phase> & phasesBus1, const Matrix<Complex> & Y) :
      id_(id),
      idBus0_(idBus0),
      idBus1_(idBus1)
   {
      int nPhase = phasesBus0.size();
      int nTerm = 2 * nPhase;
      assert(phasesBus1.size() == nPhase);
      assert(Y.size() == nTerm);

      // There is a single link for every distinct pair of terminals.
      for (int i0 = 0; i0 < nTerm; ++i0)
      {
         for (int i1 = i0 + 1; i1 < nTerm; ++i1)
         {
            Array2D<Complex, 2, 2> YLink;
            YLink[0][0] = Y(i0, i0);
            YLink[0][1] = Y(i0, i1);
            YLink[1][0] = Y(i1, i0);
            YLink[1][1] = Y(i1, i1);

            int busIdx0 = i0 / nPhase;
            int busIdx1 = i1 / nPhase;
            int phaseIdx0 = i0 % nPhase;
            int phaseIdx1 = i1 % nPhase;
            links_.emplace_back(*this, busIdx0, busIdx1, phaseIdx0, phaseIdx1, YLink);
         }
      }
   }

   LinkNR::LinkNR(const Branch & branch, int busIdx0, int busIdx0, int phaseIdx1, int phaseIdx1, 
         const Array2D<Complex, 2, 2> & Y) :
      branch_(&branch),
      busIdxs_{busIdx0, busIdx1},
      phaseIdxs_{phaseIdx0, phaseIdx1},
      Y_(Y),
      nodei_(nullptr),
      nodek_(nullptr)
   {
      // Empty.
   }

   void PowerFlowNR::addBus(const std::string & id, BusType type, const Vector<Phase> & phases,
         const Vector<Complex> & V, const Vector<Complex> & Y, Vector<Complex> & I, Vector<Complex> & S)
   {
      SGT_DEBUG(debug() << "PowerFlowNR : addBus " << id << std::endl);
      BusNR * bus = new BusNR(id, type, phases, V, Y, I, S);
      bussesById_[bus->id_] = bus;
   }

   void PowerFlowNR::addBranch(const std::string & id, const std::string & idBus0, const std::string & idBus1,
         const Vector<Phase> & phasesBus0, const Vector<Phase> & phasesBus1, const Matrix<Complex> & Y)
   {
      SGT_DEBUG(debug() << "PowerFlowNR : addBranch " << id << " " << idBus0 << " " << idBus1 << std::endl);
      BranchNR * branch = new BranchNR(id, idBus0, idBus1, phasesBus0, phasesBus1, Y);
      branches_.push_back(branch);
   }

   void PowerFlowNR::reset()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : reset." << std::endl);
      for (auto & pair : bussesById_) delete pair.second;
      for (auto branch : branches_) delete branch;
      busses_ = BusVec();
      SLBusses_ = BusVec();
      PQBusses_ = BusVec();
      bussesById_ = BusMap();
      branches_ = BranchVec();
   }

   void PowerFlowNR::validate()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : validate." << std::endl);
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

      // Index all PQ busses:
      for (int i = 0; i < nBus_; ++i)
      {
         busses_[i]->idx_ = i;
      }

      // Set bus pointers in all branches.
      for (BranchNR * branch : branches_)
      {
         auto iti = bussesById_.find(branch->idBus0_);
         if (iti == bussesById_.end())
         {
            error() << "Branch " << branch->idBus0_ << " " << branch->idBus1_ << " contains a non-existent bus " 
                    << branch->idBus0_ << std::endl;
            abort();
         }
         else
         {
            branch->bus0_ = iti->second;
         }
         auto itk = bussesById_.find(branch->idBus1_);
         if (itk == bussesById_.end())
         {
            error() << "Branch " << branch->idBus0_ << " " << branch->idBus1_ << " contains a non-existent bus "
                    << branch->idBus1_ << std::endl;
            abort();
         }
         else
         {
            branch->bus1_ = itk->second;
         }
      }

      // Set array ranges:
      // Note Range goes from begin (included) to end (excluded).
      rPQ_ = UblasRange(0, nPQ_);
      rAll_ = UblasRange(0, nPQ_ + 1);
      iSL_ = nPQ_;
      rx0_ = UblasRange(0, nPQ_);
      rx1_ = UblasRange(nPQ_, 2 * nPQ_);

      // Size all arrays:
      PPQ_.resize(nPQ_, false);
      QPQ_.resize(nPQ_, false);
      IrPQ_.resize(nPQ_, false);
      IiPQ_.resize(nPQ_, false);
      Vr_.resize(nBus_, false);
      Vi_.resize(nBus_, false);
      Y_.resize(nBus_, nBus_, false);
      G_.resize(nBus_, nBus_, false);
      B_.resize(nBus_, nBus_, false);
      x_.resize(nVar_, false);
      f_.resize(nVar_, false);
      J_.resize(nVar_, nVar_, false);
      JConst_.resize(nVar_, nVar_, false);

      // Set the slack voltages:
      V0_ = SLBusses_[0]->V_; // Array copy.

      for (int i = 0; i < nPQ_; ++i)
      {
         // Set the PPQ_ and QPQ_ arrays of real and reactive power on each terminal:
         PPQ_(i) = (busses_[i]->S_).real();
         QPQ_(i) = (busses_[i]->S_).imag();
         // Set the constant current arrays of real and reactive power on each terminal:
         IrPQ_(i) = (busses_[i]->I_).real();
         IiPQ_(i) = (busses_[i]->I_).imag();
      }

      // Build the bus admittance matrix:
      buildBusAdmit();

      // And set G_ and B_:
      G_ = real(Y_);
      B_ = imag(Y_);

      // Set the part of J that doesn't update at each iteration.
      JConst_.clear();
      UblasCMatrixRange<double>(JConst_, rx0_, rx0_) = UblasCMatrixRange<double>(G_, rPQ_, rPQ_);
      UblasCMatrixRange<double>(JConst_, rx0_, rx1_) = -UblasCMatrixRange<double>(B_, rPQ_, rPQ_);
      UblasCMatrixRange<double>(JConst_, rx1_, rx0_) = UblasCMatrixRange<double>(B_, rPQ_, rPQ_);
      UblasCMatrixRange<double>(JConst_, rx1_, rx1_) = UblasCMatrixRange<double>(G_, rPQ_, rPQ_);
      J_ = JConst_; // We only need to redo the elements that we mess with!
#ifdef DEBUG
      outputNetwork();
#endif
   }

   void PowerFlowNR::buildBusAdmit()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : buildBusAdmit." << std::endl);
      // Zero the matrix:
      Y_.clear();

      // Add branch admittances.
      for (const BranchNR * const branch : branches_)
      {
         const BusNR * bus0 = branch->bus0_;
         const BusNR * bus1 = branch->bus1_;

         int ibus0 = bus0->idx_;
         int ibus1 = bus1->idx_;

         Y_(ibus0, ibus0) += branch->Y_[0][0];
         Y_(ibus1, ibus1) += branch->Y_[1][1];
         Y_(ibus0, ibus1) += branch->Y_[0][1];
         Y_(ibus1, ibus0) += branch->Y_[1][0];
      }

      // Add bus shunt admittances.
      for (const BusNR * const bus : busses_)
      {
         int ibus0 = bus->idx_;
         Y_(ibus0, ibus0) += bus->Y_;
      }
   }

   void PowerFlowNR::initx()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : initx." << std::endl);
      for (int i = 0; i < nPQ_; ++i)
      {
         const BusNR & bus = *busses_[i + 1];
         x_(i) = V0_.real();
         x_(i + nPQ_) = V0_.imag();
      }
   }

   void PowerFlowNR::updateBusV()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : updateBusV." << std::endl);
      // Get voltages on all busses.
      // Done like this with a copy because eventually we'll include PV busses too.
      UblasVectorRange<double>(Vr_, rPQ_) = UblasVectorRange<double>(x_, rx0_);
      UblasVectorRange<double>(Vi_, rPQ_) = UblasVectorRange<double>(x_, rx1_);
      Vr_(iSL_) = V0_.real();
      Vi_(iSL_) = V0_.imag();
   }

   void PowerFlowNR::updateF()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : updateF." << std::endl);
      UblasVectorRange<double> x0{x_, rx0_};
      UblasVectorRange<double> x1{x_, rx1_};

      updateBusV();

      UblasVector<double> M2 = element_prod(x0, x0) + element_prod(x1, x1);

      UblasCMatrixRange<double> GRng{G_, rPQ_, rAll_};
      UblasCMatrixRange<double> BRng{B_, rPQ_, rAll_};

      UblasVector<double> dr = element_div((-element_prod(PPQ_, x0) - element_prod(QPQ_, x1)), M2)
                               + prod(GRng, Vr_) - prod(BRng, Vi_) - IrPQ_;
      UblasVector<double> di = element_div((-element_prod(PPQ_, x1) + element_prod(QPQ_, x0)), M2)
                               + prod(GRng, Vi_) + prod(BRng, Vr_) - IiPQ_;

      UblasVectorRange<double>(f_, rx0_) = dr;
      UblasVectorRange<double>(f_, rx1_) = di;
   }

   void PowerFlowNR::updateJ()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : updateJ." << std::endl);
      UblasVectorRange<double> x0{x_, rx0_};
      UblasVectorRange<double> x1{x_, rx1_};

      updateBusV();

      UblasVector<double> M2PQ = element_prod(x0, x0) + element_prod(x1, x1);
      UblasVector<double> M4PQ = element_prod(M2PQ, M2PQ);

      for (int i = 0; i < nPQ_; ++i)
      {
         J_(i, i) = JConst_(i, i) +
            (-PPQ_(i) / M2PQ(i) + 2 * Vr_(i) * (PPQ_(i) * Vr_(i) + QPQ_(i) * Vi_(i)) / M4PQ(i));
         J_(i, i + nPQ_) = JConst_(i, i + nPQ_) +
            (-QPQ_(i) / M2PQ(i) + 2 * Vi_(i) * (PPQ_(i) * Vr_(i) + QPQ_(i) * Vi_(i)) / M4PQ(i));
         J_(i + nPQ_, i) = JConst_(i + nPQ_, i) +
            ( QPQ_(i) / M2PQ(i) + 2 * Vr_(i) * (PPQ_(i) * Vi_(i) - QPQ_(i) * Vr_(i)) / M4PQ(i));
         J_(i + nPQ_, i + nPQ_) = JConst_(i + nPQ_, i + nPQ_) +
            (-PPQ_(i) / M2PQ(i) + 2 * Vi_(i) * (PPQ_(i) * Vi_(i) - QPQ_(i) * Vr_(i)) / M4PQ(i));
      }
   }

   bool PowerFlowNR::solve()
   {
      SGT_DEBUG(debug() << "PowerFlowNR : solve." << std::endl);
      const double tol = 1e-20;
      const int maxiter = 20;
      initx();
      bool wasSuccessful = false;
      for (int i = 0; i < maxiter; ++ i)
      {
         updateF();
         updateJ();
         UblasVector<double> rhs;
         KLUSolve(J_, f_, rhs);
         x_ = x_ - rhs;
         UblasVector<double> test = prod(J_, x_) - f_;
         UblasVector<double> f2 = element_prod(f_, f_);
         double err = *std::max_element(f2.begin(), f2.end());
         SGT_DEBUG(debug() << "Error at iteration " << i << " = " << err << std::endl);
         if (err <= tol)
         {
            SGT_DEBUG(debug() << "Success at iteration " << i << ". Error = " << err << std::endl);
            wasSuccessful = true;
            break;
         }
      }
      if (wasSuccessful)
      {
         for (int i = 0; i < nPQ_; ++i)
         {
            PQBusses_[i]->V_ = {x_(i), x_(i + nPQ_)};
         }
         // TODO: set power e.g. on slack bus. Set current injections. Set impedances to ground. 
      }
      return wasSuccessful;
   }

   void PowerFlowNR::outputNetwork()
   {
      SGT_DEBUG(debug() << "Number of busses = " << nBus_ << std::endl);
      SGT_DEBUG(debug() << "Number of PQ busses = " << nPQ_ << std::endl);
      SGT_DEBUG(debug() << "Number of slack busses = " << nSL_ << std::endl);
      for (const BusNR * bus : busses_)
      {
         SGT_DEBUG(debug() << "\tBus: " << bus->idx_ << " " << bus->id_ << " " << (int)bus->type_ << " " 
               << bus->V_ << " " << bus->Y_ << " " << bus->I_ << " " << bus->S_ << std::endl);
      }
      for (BranchNR * branch : branches_)
      {
         SGT_DEBUG(debug() << "\tBranch: " << branch->idBus0_ << " " << branch->idBus1_ << std::endl);
         for (int i = 0; i < 2; ++i)
         {
            SGT_DEBUG(debug() << "\t\t" << branch->Y_[i][0] << " " << branch->Y_[i][1] << std::endl);
         }
      }
   }

   void PowerFlowNR::outputCurrentState()
   {
      using namespace std;
      SGT_DEBUG(debug() << "x : " << x_ << std::endl);
      SGT_DEBUG(debug() << "f : " << f_ << std::endl);
      std::ostringstream ssJ;
      ssJ << "J: " << std::endl;
      for (int i = 0; i < J_.size1(); ++i)
      {
         ssJ << "\t";
         for (int k = 0; k < J_.size2(); ++k)
         {
            ssJ << J_(i, k) << " ";
         }
         ssJ << endl;
      }
      SGT_DEBUG(debug() << ssJ.str() << std::endl);
   }
}
