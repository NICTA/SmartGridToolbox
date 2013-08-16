#include <algorithm>
#include <ostream>
#include <sstream>
#include "PowerFlow1PNR.h"
#include "SparseSolver.h"

namespace SmartGridToolbox
{
   void PowerFlow1PNR::addBus(const std::string & id, BusType type, Complex V, Complex Y, Complex I, Complex S)
   {
      SGT_DEBUG(debug() << "PowerFlow1PNR : add bus " << id << std::endl);
      SGT_DEBUG(debug() << "\tType : " << type << std::endl);
      SGT_DEBUG(debug() << "\tV    : " << V << std::endl);
      SGT_DEBUG(debug() << "\tY    : " << Y << std::endl);
      SGT_DEBUG(debug() << "\tI    : " << I << std::endl);
      SGT_DEBUG(debug() << "\tS    : " << S << std::endl);
      Bus1PNR * bus = new Bus1PNR;
      bus->id_ = id;
      bus->type_ = type;
      bus->V_ = V;
      bus->Y_ = Y;
      bus->I_ = I;
      bus->S_ = S;
      bus->idx_ = -1;
      bussesById_[bus->id_] = bus;
      switch (bus->type_)
      {
         case BusType::SL :
            SLBusses_.push_back(bus);
            break;
         case BusType::PQ :
            PQBusses_.push_back(bus);
            break;
         case BusType::PV :
            error() << "PV busses are not supported yet." << std::endl;
            abort();
            break;
      }
   }

   void PowerFlow1PNR::addBranch(const std::string & idi, const std::string & idk, 
                                       const Array2D<Complex, 2, 2> & Y)
   {
      SGT_DEBUG(debug() << "PowerFlow1PNR : addBranch " << idi << " " << idk << std::endl);
      SGT_DEBUG(debug() << "\tY = :" << std::endl);
      SGT_DEBUG(debug() << "\t\t" << Y[0][0] << ", " << Y[0][1] << std::endl);
      SGT_DEBUG(debug() << "\t\t" << Y[1][0] << ", " << Y[1][1] << std::endl);
      Branch1PNR * branch = new Branch1PNR;
      branch->Y_ = Y;
      branch->idi_ = idi;
      branch->idk_ = idk;
      branches_.push_back(branch);
   }

   void PowerFlow1PNR::reset()
   {
      SGT_DEBUG(debug() << "PowerFlow1PNR : reset." << std::endl);
      for (Bus1PNR * bus : busses_) delete bus;
      for (Branch1PNR * branch : branches_) delete branch;
      SLBusses_ = BusVec();
      PQBusses_ = BusVec();
      bussesById_ = BusMap();
      branches_ = BranchVec();
   }

   void PowerFlow1PNR::validate()
   {
      SGT_DEBUG(debug() << "PowerFlow1PNR : validate." << std::endl);
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
      for (Branch1PNR * branch : branches_)
      {
         auto iti = bussesById_.find(branch->idi_);
         if (iti == bussesById_.end())
         {
            error() << "Branch " << branch->idi_ << " " << branch->idk_ << " contains a non-existent bus " 
                    << branch->idi_ << std::endl;
            abort();
         }
         else
         {
            branch->busi_ = iti->second;
         }
         auto itk = bussesById_.find(branch->idk_);
         if (itk == bussesById_.end())
         {
            error() << "Branch " << branch->idi_ << " " << branch->idk_ << " contains a non-existent bus "
                    << branch->idk_ << std::endl;
            abort();
         }
         else
         {
            branch->busk_ = itk->second;
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
   }

   void PowerFlow1PNR::buildBusAdmit()
   {
      SGT_DEBUG(debug() << "PowerFlow1PNR : buildBusAdmit." << std::endl);
      // Zero the matrix:
      Y_.clear();

      // Add branch admittances.
      for (const Branch1PNR * const branch : branches_)
      {
         const Bus1PNR * busi = branch->busi_;
         const Bus1PNR * busk = branch->busk_;

         int ibus = busi->idx_;
         int kbus = busk->idx_;

         Y_(ibus, ibus) += branch->Y_[0][0];
         Y_(kbus, kbus) += branch->Y_[1][1];
         Y_(ibus, kbus) += branch->Y_[0][1];
         Y_(kbus, ibus) += branch->Y_[1][0];
      }

      // Add bus shunt admittances.
      for (const Bus1PNR * const bus : busses_)
      {
         int ibus = bus->idx_;
         Y_(ibus, ibus) += bus->Y_;
      }
   }

   void PowerFlow1PNR::initx()
   {
      SGT_DEBUG(debug() << "PowerFlow1PNR : initx." << std::endl);
      for (int i = 0; i < nPQ_; ++i)
      {
         const Bus1PNR & bus = *busses_[i + 1];
         x_(i) = V0_.real();
         x_(i + nPQ_) = V0_.imag();
      }
   }

   void PowerFlow1PNR::updateBusV()
   {
      SGT_DEBUG(debug() << "PowerFlow1PNR : updateBusV." << std::endl);
      // Get voltages on all busses.
      // Done like this with a copy because eventually we'll include PV busses too.
      UblasVectorRange<double>(Vr_, rPQ_) = UblasVectorRange<double>(x_, rx0_);
      UblasVectorRange<double>(Vi_, rPQ_) = UblasVectorRange<double>(x_, rx1_);
      Vr_(iSL_) = V0_.real();
      Vi_(iSL_) = V0_.imag();
   }

   void PowerFlow1PNR::updateF()
   {
      SGT_DEBUG(debug() << "PowerFlow1PNR : updateF." << std::endl);
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

   void PowerFlow1PNR::updateJ()
   {
      SGT_DEBUG(debug() << "PowerFlow1PNR : updateJ." << std::endl);
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

   bool PowerFlow1PNR::solve()
   {
      SGT_DEBUG(debug() << "PowerFlow1PNR : solve." << std::endl);
      SGT_DEBUG(printProblem());
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
   bool PowerFlow1PNR::printProblem()
   {
      debug() << "PowerFlow1PNR::printProblem()" << std::endl;
      debug() << "\tBusses:" << std::endl;
      for (int i = 0; i < nBus_; ++i)
      {
         Bus1PNR & bus = *busses_[i];
         debug() << "\t\tId    : " << bus.id_ << std::endl;
         debug() << "\t\t\tType : " << bus.type_ << std::endl;
         debug() << "\t\t\tV    : " << bus.V_ << std::endl;
         debug() << "\t\t\tY    : " << bus.Y_ << std::endl;
         debug() << "\t\t\tI    : " << bus.I_ << std::endl;
         debug() << "\t\t\tS    : " << bus.S_ << std::endl;
      }
      debug() << "\tY:" << std::endl;
      for (int i = 0; i < Y_.size1(); ++i)
      {
         debug() << "\t\t" << row(Y_, i) << std::endl;
      }
   }
}
