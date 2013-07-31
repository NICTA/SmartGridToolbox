#include <algorithm>
#include <ostream>
#include <sstream>
#include "PowerFlow1PNR.h"
#include "SparseSolver.h"

namespace SmartGridToolbox
{
   void PowerFlow1PNR::addBus(const std::string & id, BusType type, Complex V, Complex Y, Complex I, Complex S)
   {
      SGTDebug("PowerFlow1PNR : addBus " << id);
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
      SGTDebug("PowerFlow1PNR : addBranch " << idi << " " << idk);
      Branch1PNR * branch = new Branch1PNR;
      branch->Y_ = Y;
      branch->idi_ = idi;
      branch->idk_ = idk;
      branches_.push_back(branch);
   }

   void PowerFlow1PNR::reset()
   {
      SGTDebug("PowerFlow1PNR : reset.");
      for (Bus1PNR * bus : busses_) delete bus;
      for (Branch1PNR * bus : branches_) delete bus;
      busses_ = BusVec();
      SLBusses_ = BusVec();
      PQBusses_ = BusVec();
      bussesById_ = BusMap();
      branches_ = BranchVec();
   }

   void PowerFlow1PNR::validate()
   {
      SGTDebug("PowerFlow1PNR : validate.");
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

      // Set the PPQ_ and QPQ_ arrays of real and reactive power on each terminal:
      for (int i = 0; i < nPQ_; ++i)
      {
         PPQ_(i) = (busses_[i]->S_).real();
         QPQ_(i) = (busses_[i]->S_).imag();
      }

      // Build the bus admittance matrix:
      buildBusAdmit();

      // And set G_ and B_:
      G_ = real(Y_);
      B_ = imag(Y_);

      // Set the part of J that doesn't update at each iteration.
      UblasCMatrixRange<double>(JConst_, rx0_, rx0_) = UblasCMatrixRange<double>(G_, rPQ_, rPQ_);
      UblasCMatrixRange<double>(JConst_, rx0_, rx1_) = -UblasCMatrixRange<double>(B_, rPQ_, rPQ_);
      UblasCMatrixRange<double>(JConst_, rx1_, rx0_) = UblasCMatrixRange<double>(B_, rPQ_, rPQ_);
      UblasCMatrixRange<double>(JConst_, rx1_, rx1_) = UblasCMatrixRange<double>(G_, rPQ_, rPQ_);
      J_ = JConst_; // We only need to redo the elements that we mess with!
#ifdef DEBUG
      outputNetwork();
#endif
   }

   void PowerFlow1PNR::buildBusAdmit()
   {
      SGTDebug("PowerFlow1PNR : buildBusAdmit.");
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
   }

   void PowerFlow1PNR::initx()
   {
      SGTDebug("PowerFlow1PNR : initx.");
      for (int i = 0; i < nPQ_; ++i)
      {
         const Bus1PNR & bus = *busses_[i + 1];
         x_(i) = V0_.real();
         x_(i + nPQ_) = V0_.imag();
      }
   }

   void PowerFlow1PNR::updateBusV()
   {
      SGTDebug("PowerFlow1PNR : updateBusV.");
      // Get voltages on all busses.
      // Done like this with a copy because eventually we'll include PV busses too.
      UblasVectorRange<double>(Vr_, rPQ_) = UblasVectorRange<double>(x_, rx0_);
      UblasVectorRange<double>(Vi_, rPQ_) = UblasVectorRange<double>(x_, rx1_);
      Vr_(iSL_) = V0_.real();
      Vi_(iSL_) = V0_.imag();
   }

   void PowerFlow1PNR::updateF()
   {
      SGTDebug("PowerFlow1PNR : updateF.");
      UblasVectorRange<double> x0{x_, rx0_};
      UblasVectorRange<double> x1{x_, rx1_};

      updateBusV();

      UblasVector<double> M2 = element_prod(x0, x0) + element_prod(x1, x1);

      UblasCMatrixRange<double> GRng{G_, rPQ_, rAll_};
      UblasCMatrixRange<double> BRng{B_, rPQ_, rAll_};

      UblasVector<double> dr = element_div((-element_prod(PPQ_, x0) - element_prod(QPQ_, x1)), M2)
                               + prod(GRng, Vr_) - prod(BRng, Vi_);
      UblasVector<double> di = element_div((-element_prod(PPQ_, x1) + element_prod(QPQ_, x0)), M2)
                               + prod(GRng, Vi_) + prod(BRng, Vr_);

      UblasVectorRange<double>(f_, rx0_) = dr;
      UblasVectorRange<double>(f_, rx1_) = di;
   }

   void PowerFlow1PNR::updateJ()
   {
      SGTDebug("PowerFlow1PNR : updateJ.");
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
      SGTDebug("PowerFlow1PNR : solve.");
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
         SGTDebug("Error at iteration " << i << " = " << err);
         if (err <= tol)
         {
            SGTDebug("Success at iteration " << i << ". Error = " << err);
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

   void PowerFlow1PNR::outputNetwork()
   {
      SGTDebug("Number of busses = " << nBus_);
      SGTDebug("Number of PQ busses = " << nPQ_);
      SGTDebug("Number of slack busses = " << nSL_);
      for (const Bus1PNR * bus : busses_)
      {
         SGTDebug("    Bus: " << bus->idx_ << " " << bus->id_ << " " << (int)bus->type_ << " " << complex2String(bus->V_)
               << " " << complex2String(bus->Y_) << " " << complex2String(bus->I_) << " " << complex2String(bus->S_));
      }
      for (Branch1PNR * branch : branches_)
      {
         SGTDebug("    Branch: " << branch->idi_ << " " << branch->idk_);
         for (int i = 0; i < 2; ++i)
         {
            SGTDebug("            " << complex2String(branch->Y_[i][0]) << " " << complex2String(branch->Y_[i][1]));
         }
      }
   }

   void PowerFlow1PNR::outputCurrentState()
   {
      using namespace std;
      SGTDebug("x : " << x_);
      SGTDebug("f : " << f_);
      std::ostringstream ssJ;
      ssJ << "J: " << std::endl;
      for (int i = 0; i < J_.size1(); ++i)
      {
         ssJ << "   ";
         for (int k = 0; k < J_.size2(); ++k)
         {
            ssJ << J_(i, k) << " ";
         }
         ssJ << endl;
      }
      SGTDebug(ssJ.str());
   }
}
