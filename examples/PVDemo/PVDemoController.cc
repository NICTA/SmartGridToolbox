#include "PVDemoController.h"

namespace PVDemo
{
   using namespace SmartGridToolbox;
   
   static PVDemoInverter * getSpecialInverter(Bus & bus)
   {
      PVDemoInverter * result = nullptr;
      for (ZipToGroundBase * zip : bus.zipsToGround())
      {
         result = dynamic_cast<PVDemoInverter *>(zip);
         if (result != nullptr)
         {
            break;
         }
      }
      return result;
   }

   PVDemoController::PVDemoController(const std::string & name, Network & network,
                                      double voltageLb, double voltageUb)
      : Component(name),
        network_(&network),
        voltageLb_(voltageLb),
        voltageUb_(voltageUb),
        grbEnv_(nullptr),
        grbMod_(nullptr),
        nVars_(0),
        iter_(0)
   {
      network_->dependsOn(*this); // Network will go after me.
      GRBloadenv(&grbEnv_, "gurobi.log");
      assert(grbEnv_);
   }

   std::vector<const PVDemoBus *> PVDemoController::busses()
   {
      std::vector<const PVDemoBus *> result;
      std::transform(busses_.begin(), busses_.end(),
                     std::back_inserter(result), [](decltype(*busses_.begin()) & p) {return &p.second;});
      return result;
   }
   
   std::vector<const PVDemoBranch *> PVDemoController::branches()
   {
      std::vector<const PVDemoBranch *> result;
      std::transform(branches_.begin(), branches_.end(),
                     std::back_inserter(result), [](decltype(*branches_.begin()) & p) {return &p.second;});
      return result;
   }

   void PVDemoController::initializeState()
   {
      for (Bus * bus : network_->busVec())
      {
         addBus(*bus);
      }

      for (Branch * branch : network_->branchVec())
      {
         addBranch(*branch);
      }

      linkBussesAndBranches();
   }

   void PVDemoController::updateState(Time t0, Time t1)
   {
      createGrbModel();

      if (iter_ < 20)
      {
         std::string name = "grb_model_" + std::to_string(iter_) + ".lp";
         GRBwrite(grbMod_, name.c_str());
         ++iter_;
      }

      GRBoptimize(grbMod_);

      int status;
      GRBgetintattr(grbMod_, "Status", &status);
      if (status == GRB_OPTIMAL)
      {
         SGT_DEBUG(printSolution(););
         retrieveResults();
      }
      else
      {
         warning() << "Model was infeasible. No action taken." << std::endl;
      }
   }

   void PVDemoController::addBus(Bus & bus)
   {
      dependsOn(bus);
      bus.didUpdate().addAction([this](){needsUpdate().trigger();}, "Trigger " + name() + " needsUpdate");  
      PVDemoBus & pvdBus = busses_[bus.name()];
      pvdBus.sgtBus_ = &bus;
      pvdBus.specialInverter_ = getSpecialInverter(bus);
      if (pvdBus.specialInverter_)
      {
         dependsOn(*pvdBus.specialInverter_);
      }
   }

   void PVDemoController::addBranch(Branch & branch)
   {
      dependsOn(branch);
      PVDemoBranch & pvdBranch = branches_[branch.name()];
      pvdBranch.sgtBranch_ = &branch;
   }

   void PVDemoController::linkBussesAndBranches()
   {
      for (auto & pair : branches_)
      {
         PVDemoBranch & pvdBranch = pair.second;
         PVDemoBus & pvdBus0 = busses_[pvdBranch.sgtBranch_->bus0().name()];
         PVDemoBus & pvdBus1 = busses_[pvdBranch.sgtBranch_->bus1().name()];
         pvdBranch.pvdBus0_ = &pvdBus0;
         pvdBranch.pvdBus1_ = &pvdBus1;
         pvdBus0.pvdBranches_.insert(&pvdBranch);
         pvdBus1.pvdBranches_.insert(&pvdBranch);
      };
   };

   void PVDemoController::recreateBusData()
   {
      for (auto & pair : busses_)
      {
         PVDemoBus & pvdBus = pair.second;
         pvdBus.SLoad_ = pvdBus.sgtBus_->Sc()(0);
         pvdBus.SGen_ = pvdBus.sgtBus_->Sg()(0);
         pvdBus.SMagMax_ = 0.0;
         if (pvdBus.specialInverter_)
         {
            // Count inverter as generator, not load.
            pvdBus.SGen_ += pvdBus.specialInverter_->SimpleInverter::S()(0);
            pvdBus.SLoad_ -= pvdBus.specialInverter_->SimpleInverter::S()(0);
            pvdBus.SMagMax_ = pvdBus.specialInverter_->maxSMagPerPhase();
         }

         SGT_DEBUG(debug() << "Add Bus: " << pvdBus.sgtBus_->name() << " " << pvdBus.SLoad_ << " "
                           << pvdBus.SGen_ << std::endl);

         pvdBus.thetaIdx_ = addVar(pvdBus.sgtBus_->name() + "_theta", -GRB_INFINITY, GRB_INFINITY, 0.0);
         pvdBus.phiIdx_ = addVar(pvdBus.sgtBus_->name() + "_phi", -1.0, GRB_INFINITY, 0.0);
         pvdBus.phiSlackLbIdx_ = addVar(pvdBus.sgtBus_->name() + "_phi_slack_lb", 0.0, GRB_INFINITY, 1e6);
         pvdBus.phiSlackUbIdx_ = addVar(pvdBus.sgtBus_->name() + "_phi_slack_ub", 0.0, GRB_INFINITY, 1e6);

         if (pvdBus.specialInverter_)
         {
            pvdBus.aGenIdx_ = addVar(pvdBus.sgtBus_->name() + "_active", 0.0, pvdBus.SGen_.real(), 0.0);
            pvdBus.rGenIdx_ = addVar(pvdBus.sgtBus_->name() + "_reactive",
                                     pvdBus.sgtBus_->QgMinSetpoint()(0), pvdBus.sgtBus_->QgMaxSetpoint()(0), 0.0);
            // Use bus bounds to agree with xcode in special cases. Alternative: make the variable free and rely on the
            // inverter's max apparent power constraints.
         }
         else if (pvdBus.sgtBus_->type() == BusType::SL)
         {
            pvdBus.aGenIdx_ = addVar(pvdBus.sgtBus_->name() + "_active", 0.0, GRB_INFINITY, 1.0e4);
            pvdBus.rGenIdx_ = addVar(pvdBus.sgtBus_->name() + "_reactive", -GRB_INFINITY, GRB_INFINITY, 0.0);
         }
         else if (pvdBus.sgtBus_->type() == BusType::PV)
         {
            pvdBus.aGenIdx_ = addVar(pvdBus.sgtBus_->name() + "_active", 0.0, pvdBus.SGen_.real(), 1.0e4);
            pvdBus.rGenIdx_ = addVar(pvdBus.sgtBus_->name() + "_reactive", -GRB_INFINITY, GRB_INFINITY, 0.0);
         }
         else if (pvdBus.sgtBus_->type() == BusType::PQ)
         {
            pvdBus.aGenIdx_ = addVar(pvdBus.sgtBus_->name() + "_active", 0.0, pvdBus.SGen_.real(), 1.0e4);
            pvdBus.rGenIdx_ = addVar(pvdBus.sgtBus_->name() + "_reactive", 0.0, 0.0, 0.0);
         }
      }
   }

   void PVDemoController::recreateBranchData()
   {
      for (auto & pair : branches_)
      {
         PVDemoBranch & pvdBranch = pair.second;
         pvdBranch.y_ = -pvdBranch.sgtBranch_->Y()(0, 1); // TODO : assumes not a transformer!
         pvdBranch.aFlowFIdx_ = addVar(pvdBranch.sgtBranch_->name() + "_activeF", -GRB_INFINITY, GRB_INFINITY, 0.0);
         pvdBranch.aFlowRIdx_ = addVar(pvdBranch.sgtBranch_->name() + "_activeR", -GRB_INFINITY, GRB_INFINITY, 0.0);
         pvdBranch.rFlowFIdx_ = addVar(pvdBranch.sgtBranch_->name() + "_reactiveF", -GRB_INFINITY, GRB_INFINITY, 0.0);
         pvdBranch.rFlowRIdx_ = addVar(pvdBranch.sgtBranch_->name() + "_reactiveR", -GRB_INFINITY, GRB_INFINITY, 0.0);
         pvdBranch.cosIdx_ = addVar(pvdBranch.sgtBranch_->name() + "_cos", 0.0, 1.0, -1.0);
      }
   }

   void PVDemoController::createGrbModel()
   {
      if (grbMod_)
      {
         GRBfreemodel(grbMod_); grbMod_ = nullptr;
      }
      GRBnewmodel(grbEnv_, &grbMod_, "power_flow", 0, 0, 0, 0, 0, 0);
      GRBsetintattr(grbMod_, "ModelSense", 1); // Minimise costs.

      nVars_ = 0;
      recreateBusData();
      recreateBranchData();
      GRBupdatemodel(grbMod_);

      // All variables have been added, now add constraints.
      for (const auto & pair : branches_)
      {
         const PVDemoBranch & pvdBranch = pair.second;
         postCosineConvexHull(pvdBranch.cosIdx_, pvdBranch.pvdBus0_->thetaIdx_, pvdBranch.pvdBus1_->thetaIdx_,
                              -1.0, 1.0, 500);

         int cindaf[] = {pvdBranch.cosIdx_, pvdBranch.pvdBus0_->thetaIdx_, pvdBranch.pvdBus1_->thetaIdx_, 
                         pvdBranch.aFlowFIdx_};
         double cvalaf[] = {pvdBranch.y_.real(), pvdBranch.y_.imag(), -pvdBranch.y_.imag(), 1.0};
         addConstr(4, cindaf, cvalaf, GRB_EQUAL, pvdBranch.y_.real());

         int cindar[] = {pvdBranch.cosIdx_, pvdBranch.pvdBus0_->thetaIdx_, pvdBranch.pvdBus1_->thetaIdx_,
                         pvdBranch.aFlowRIdx_};
         double cvalar[] = {pvdBranch.y_.real(), -pvdBranch.y_.imag(), pvdBranch.y_.imag(), 1.0};
         addConstr(4, cindar, cvalar, GRB_EQUAL, pvdBranch.y_.real());

         int cindrf[] = {pvdBranch.cosIdx_, pvdBranch.pvdBus0_->thetaIdx_, pvdBranch.pvdBus1_->thetaIdx_,
                         pvdBranch.pvdBus0_->phiIdx_, pvdBranch.pvdBus1_->phiIdx_, pvdBranch.rFlowFIdx_};
         double cvalrf[] = {-pvdBranch.y_.imag(), pvdBranch.y_.real(), -pvdBranch.y_.real(), 
            pvdBranch.y_.imag(), -pvdBranch.y_.imag(), 1.0};
         addConstr(6, cindrf, cvalrf, GRB_EQUAL, -pvdBranch.y_.imag());

         int cindrr[] = {pvdBranch.cosIdx_, pvdBranch.pvdBus0_->thetaIdx_, pvdBranch.pvdBus1_->thetaIdx_,
                         pvdBranch.pvdBus0_->phiIdx_, pvdBranch.pvdBus1_->phiIdx_, pvdBranch.rFlowRIdx_};
         double cvalrr[] = {-pvdBranch.y_.imag(), -pvdBranch.y_.real(), pvdBranch.y_.real(), -pvdBranch.y_.imag(),
                            pvdBranch.y_.imag(), 1.0};
         addConstr(6, cindrr, cvalrr, GRB_EQUAL, -pvdBranch.y_.imag());
      }

      for (const auto & pair : busses_)
      {
         const PVDemoBus & pvdBus = pair.second;

         int nLines = pvdBus.pvdBranches_.size();

         std::vector<int> cinda(nLines + 1);
         std::vector<double> cvala(nLines + 1);

         std::vector<int> cindr(nLines + 1);
         std::vector<double> cvalr(nLines + 1);

         int i = 0;

         for (const PVDemoBranch * pvdBranch : pvdBus.pvdBranches_)
         {
            PVDemoBus * fromBus = pvdBranch->pvdBus0_;
            int activeLineIndex = -1;
            int reactiveLineIndex = -1;
            if (&pvdBus == fromBus)
            {
               activeLineIndex = pvdBranch->aFlowFIdx_;
               reactiveLineIndex = pvdBranch->rFlowFIdx_;
            } 
            else
            {
               activeLineIndex = pvdBranch->aFlowRIdx_;
               reactiveLineIndex = pvdBranch->rFlowRIdx_;
            }

            cinda[i] = activeLineIndex;
            cvala[i] = 1.0;

            cindr[i] = reactiveLineIndex;
            cvalr[i] = 1.0;

            ++i;
         }

         cinda[i] = pvdBus.aGenIdx_;
         cvala[i] = -1.0;

         cindr[i] = pvdBus.rGenIdx_;
         cvalr[i] = -1.0;

         // Load constraints.
         addConstr(nLines+1, cinda.data(), cvala.data(), GRB_EQUAL, pvdBus.SLoad_.real());
         addConstr(nLines+1, cindr.data(), cvalr.data(), GRB_EQUAL, pvdBus.SLoad_.imag());

         if (pvdBus.specialInverter_)
         {
            // Special bus, P + Q <= min(PGen, QgMaxSetpoint).
            // TODO: this is the weird constraint.
            /*
            double ub = std::min(pvdBus.SGen_.real(), pvdBus.sgtBus_->QgMaxSetpoint()(0));
            int cind[] = {pvdBus.aGenIdx_, pvdBus.rGenIdx_};
            double cval[] = {1.0, 1.0};
            addConstr(2, cind, cval, GRB_LESS_EQUAL, ub);
            */
            postSemicircleConvexHull(pvdBus.aGenIdx_, pvdBus.rGenIdx_, pvdBus.SMagMax_, 500);
         }
         else if (pvdBus.sgtBus_->type() == BusType::SL)
         {
            // Slack bus, theta = 0.
            int cind[] = {pvdBus.thetaIdx_};
            double cval[] = {1.0};
            addConstr(1, cind, cval, GRB_EQUAL, 0);
         }

         if (pvdBus.sgtBus_->type() == BusType::SL || pvdBus.sgtBus_->type() == BusType::PV)
         {
            // Slack or PV bus, phi = |V| - 1.0.
            int cind[] = {pvdBus.phiIdx_};
            double cval[] = {1.0};
            addConstr(1, cind, cval, GRB_EQUAL, abs(pvdBus.sgtBus_->V()(0)) - 1.0);
         } 
         else
         {
            // normal or special PQ bus |V| <= voltageUb_.
            int cind[] = {pvdBus.phiIdx_, pvdBus.phiSlackUbIdx_};
            double cval[] = {1.0, -1.0};
            addConstr(2, cind, cval, GRB_LESS_EQUAL, voltageUb_ - 1.0);
            //int cind[] = {pvdBus.phiIdx_};
            //double cval[] = {1.0};
            //addConstr(1, cind, cval, GRB_LESS_EQUAL, voltageUb_ - 1.0);

            // normal or special PQ bus |V| >= voltageLb_.
            int cind2[] = {pvdBus.phiIdx_, pvdBus.phiSlackLbIdx_};
            double cval2[] = {1.0, 1.0};
            addConstr(2, cind2, cval2, GRB_GREATER_EQUAL, voltageLb_ - 1.0);
            //int cind2[] = {pvdBus.phiIdx_};
            //double cval2[] = {1.0};
            //addConstr(1, cind2, cval2, GRB_GREATER_EQUAL, voltageLb_ - 1.0);
         }
      }
      GRBupdatemodel(grbMod_);
   }

   void PVDemoController::retrieveResults()
   {
      for (auto & pair : busses_)
      {
         PVDemoBus & pvdBus = pair.second;

         Bus & bus = *pvdBus.sgtBus_;

         double activeVal;
         double reactiveVal;
         double thetaVal;
         double voltageVal;

         GRBgetdblattrelement(grbMod_, "X", pvdBus.aGenIdx_, &activeVal);
         GRBgetdblattrelement(grbMod_, "X", pvdBus.rGenIdx_, &reactiveVal);
         GRBgetdblattrelement(grbMod_, "X", pvdBus.thetaIdx_, &thetaVal);
         GRBgetdblattrelement(grbMod_, "X", pvdBus.phiIdx_, &voltageVal);

         voltageVal += 1.0;

         Complex S = {activeVal, reactiveVal};
         Complex V = SmartGridToolbox::polar(voltageVal, thetaVal);
        
         SGT_DEBUG(
               debug() << "SGT name = " << pvdBus.sgtBus_->name() << std::endl;
               debug() << "SGT type = " << pvdBus.sgtBus_->type() << std::endl;
               debug() << "SLoad    = " << pvdBus.SLoad_ << std::endl;
               debug() << "SGen     = " << pvdBus.SGen_ << std::endl;
               if (pvdBus.specialInverter_)
               {
                  debug() << "Inv S    = " << pvdBus.specialInverter_->S()(0) << std::endl;
                  debug() << "SMagMax  = " << pvdBus.SMagMax_ << std::endl;
               }
               else
               {
                  debug() << "Inv S    = N/A" << std::endl;
                  debug() << "SMagMax  = N/A" << std::endl;
               }
               debug() << "SOL S    = " << S << std::endl;
               debug() << "SOL V    = " << V << std::endl;
               debug() << "SOL |V|  = " << abs(V) << std::endl;
         );

         if (pvdBus.specialInverter_)
         {
            message() << "Change " << pvdBus.sgtBus_->name() << pvdBus.specialInverter_->SimpleInverter::S()(0) 
                      << " " << S << std::endl;
            pvdBus.specialInverter_->setS(S - pvdBus.sgtBus_->Sg()(0));
               // Delta S for inverter due to optimisation.
               // Note that nonzero Sg is a slightly odd case. Normally all generation on the PQ bus will be due to
               // the inverter, but we allow for extra generation directly on the bus to check results against 
               // orignal xcode PV demo.
            pvdBus.sgtBus_->ensureValid();
         }

         switch (pvdBus.sgtBus_->type())
         {
            case BusType::SL:
               pvdBus.sgtBus_->setSg({1, S});                                       // Warm start.
               break;
            case BusType::PQ:
               pvdBus.sgtBus_->setV({1, V});                                        // Warm start.
               break;
            case BusType::PV:
               pvdBus.sgtBus_->setSg({1, Complex(pvdBus.SGen_.real(), S.imag())});  // Warm start.
               pvdBus.sgtBus_->setV({1, V * abs(pvdBus.sgtBus_->V()(0)) / abs(V)}); // Warm start.
               break;
            default:
               error() << "Bad bus type." << std::endl;
               SmartGridToolbox::abort();
               break;
         }
         
         pvdBus.SSol_ = pvdBus.sgtBus_->STot()(0);
         pvdBus.VSol_ = V;
      }
   }
   
   int PVDemoController::addVar(const std::string & name, double lb, double ub, double obj)
   {
      int ind[] = {};
      double val[] = {};
      GRBaddvar(grbMod_, 0, ind, val, obj, lb, ub, GRB_CONTINUOUS, name.c_str());
      SGT_DEBUG(debug() << "Added variable " << name << " " << lb << " " << ub << " " << obj << std::endl);
      return nVars_++;
   }
   
   void PVDemoController::addConstr(int nnz, int idxi[], double coeff[], int type, double val)
   {
      GRBaddconstr(grbMod_, nnz, idxi, coeff, type, val, nullptr);
      std::string typeStr;
      switch (type)
      {
         case GRB_EQUAL :
            typeStr = "=";
            break;
         case GRB_LESS_EQUAL :
            typeStr = "<=";
            break;
         case GRB_GREATER_EQUAL :
            typeStr = ">=";
            break;
      }
      SGT_DEBUG(
            debug() << "Added constraint: ";
            for (int i = 0; i < nnz - 1; ++i) debugStream() << coeff[i] << " * v" << idxi[i] << " + ";
            debugStream() << coeff[nnz-1] << " * v" << idxi[nnz-1] << " " << typeStr << " " << val << std::endl;);
   }
   
   void PVDemoController::postCosineConvexHull(int cindex, int findex, int tindex, double lb, double ub, int steps)
   {
      double increment = (ub-lb)/(steps+1);
      double loc = lb + increment;
      double eval = -999;
      double slope = -999;
      for (int i=0; i < steps; i++)
      {
         eval = cos(loc);
         slope = -sin(loc);

         int cindr[] = {cindex, findex, tindex};
         double cvalr[] = {1.0, -slope, slope};
         addConstr(3, cindr, cvalr, GRB_LESS_EQUAL, -slope*loc+eval);
         loc += increment;
      }
   }
   
   void PVDemoController::postSemicircleConvexHull(int aGenIdx, int rGenIdx, double radius, int extraStepsDiv2)
   {
      double val[] = {1.0};  
      addConstr(1, &aGenIdx, val, GRB_GREATER_EQUAL, 0.0); // Only right side of semicircle since reactive > 0.
      addConstr(1, &aGenIdx, val, GRB_LESS_EQUAL, radius); // Only right side of semicircle since reactive > 0.
      addConstr(1, &rGenIdx, val, GRB_GREATER_EQUAL, -radius);
      addConstr(1, &rGenIdx, val, GRB_LESS_EQUAL, radius);
      for (int i = 0; i < extraStepsDiv2; ++i)
      {
         for (int j = -1; j <= 1; j += 2)
         {
            double theta = j * (i + 1) * 0.5 * pi / (extraStepsDiv2 + 1);
            double x1 = radius * cos(theta);
            double y1 = radius * sin(theta);
            double slope = -x1 / y1;
            int idx[] = {aGenIdx, rGenIdx};
            double coeff[] = {slope, -1.0};
            addConstr(2, idx, coeff, GRB_LESS_EQUAL, abs(slope * x1 - y1));
         }
      }
   }

   void PVDemoController::printSolution()
   {
      int status;
      double obj;
      double x;
      char * vname;

      GRBgetintattr(grbMod_, "Status", &status);
      if (status == GRB_OPTIMAL)
      {
         GRBgetdblattr(grbMod_, "ObjVal", &obj);
         debug() << "Cost: " << obj << std::endl;

         debug() << "Vars: " << nVars_ << std::endl;
         for (int j = 0; j < nVars_; ++j)
         {
            GRBgetdblattrelement(grbMod_, "X", j, &x);
            GRBgetstrattrelement(grbMod_, "VarName", j, &vname);
            debug() << "(" << j << ": " << vname << " = " << x << std::endl;
         };
      }
      else
      {
         debug() << "No solution." << std::endl;
      }
   }
}
