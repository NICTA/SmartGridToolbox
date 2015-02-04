#include "PowerFlowOpfSolver.h"

#include "Branch.h"
#include "Bus.h"
#include "Gen.h"

extern "C" {
#include <gurobi_c.h>
}

namespace SmartGridToolbox
{
   void PowerFlowOpfSolver::addBus(Bus& bus)
   {
      OpfBus& opfBus = busses_[bus.id()];
      opfBus.bus_ = &bus;
   }

   void PowerFlowOpfSolver::addBranch(BranchAbc& branch)
   {
      OpfBranch& opfBranch = branches_[branch.id()];
      opfBranch.branch_ = &branch;
   }

   void PowerFlowOpfSolver::linkBussesAndBranches()
   {
      for (auto& pair : branches_)
      {
         OpfBranch& opfBranch = pair.second;
         OpfBus& opfBus0 = busses_[opfBranch.branch_->bus0()->id()];
         OpfBus& opfBus1 = busses_[opfBranch.branch_->bus1()->id()];
         opfBranch.opfBus0_ = &opfBus0;
         opfBranch.opfBus1_ = &opfBus1;
         opfBus0.opfBranches_[opfBranch.branch_->id()] = &opfBranch;
         opfBus1.opfBranches_[opfBranch.branch_->id()] = &opfBranch;
      };
   };

   void PowerFlowOpfSolver::recreateBusData()
   {
      for (auto& pair : busses_)
      {
         OpfBus& opfBus = pair.second;
         opfBus.SLoad_ = opfBus.bus_->SZip()(0);
         opfBus.SGen_ = opfBus.bus_->SGen()(0);
         opfBus.SMagMax_ = 0.0;

         SGT_DEBUG(debug() << "Add Bus: " << opfBus.bus_->id() << " " << opfBus.SLoad_ << " "
                           << opfBus.SGen_ << std::endl);

         opfBus.thetaIdx_ = addVar(opfBus.bus_->id() + "_theta", -GRB_INFINITY, GRB_INFINITY, 0.0);
         opfBus.phiIdx_ = addVar(opfBus.bus_->id() + "_phi", -1.0, GRB_INFINITY, 0.0);
         opfBus.phiSlackLbIdx_ = addVar(opfBus.bus_->id() + "_phi_slack_lb", 0.0, GRB_INFINITY, 1e6);
         opfBus.phiSlackUbIdx_ = addVar(opfBus.bus_->id() + "_phi_slack_ub", 0.0, GRB_INFINITY, 1e6);

         if (opfBus.bus_->type() == BusType::SL)
         {
            opfBus.aGenIdx_ = addVar(opfBus.bus_->id() + "_active", 0.0, GRB_INFINITY, 1.0e4);
            opfBus.rGenIdx_ = addVar(opfBus.bus_->id() + "_reactive", -GRB_INFINITY, GRB_INFINITY, 0.0);
         }
         else if (opfBus.bus_->type() == BusType::PV)
         {
            opfBus.aGenIdx_ = addVar(opfBus.bus_->id() + "_active", 0.0, opfBus.SGen_.real(), 1.0e4);
            opfBus.rGenIdx_ = addVar(opfBus.bus_->id() + "_reactive", -GRB_INFINITY, GRB_INFINITY, 0.0);
         }
         else if (opfBus.bus_->type() == BusType::PQ)
         {
            opfBus.aGenIdx_ = addVar(opfBus.bus_->id() + "_active", 0.0, opfBus.SGen_.real(), 1.0e4);
            opfBus.rGenIdx_ = addVar(opfBus.bus_->id() + "_reactive", 0.0, 0.0, 0.0);
         }
      }
   }

   void PowerFlowOpfSolver::recreateBranchData()
   {
      for (auto& pair : branches_)
      {
         OpfBranch& opfBranch = pair.second;
         opfBranch.y_ = -opfBranch.branch_->Y()(0, 1); // TODO : assumes not a transformer!
         opfBranch.aFlowFIdx_ = addVar(opfBranch.branch_->id() + "_activeF", -GRB_INFINITY, GRB_INFINITY, 0.0);
         opfBranch.aFlowRIdx_ = addVar(opfBranch.branch_->id() + "_activeR", -GRB_INFINITY, GRB_INFINITY, 0.0);
         opfBranch.rFlowFIdx_ = addVar(opfBranch.branch_->id() + "_reactiveF", -GRB_INFINITY, GRB_INFINITY, 0.0);
         opfBranch.rFlowRIdx_ = addVar(opfBranch.branch_->id() + "_reactiveR", -GRB_INFINITY, GRB_INFINITY, 0.0);
         opfBranch.cosIdx_ = addVar(opfBranch.branch_->id() + "_cos", 0.0, 1.0, -1.0);
      }
   }

   void PowerFlowOpfSolver::createGrbModel()
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
      for (const auto& pair : branches_)
      {
         const OpfBranch& opfBranch = pair.second;
         postCosineConvexHull(opfBranch.cosIdx_, opfBranch.opfBus0_->thetaIdx_, opfBranch.opfBus1_->thetaIdx_,
                              -1.0, 1.0, 500);

         int cindaf[] = {opfBranch.cosIdx_, opfBranch.opfBus0_->thetaIdx_, opfBranch.opfBus1_->thetaIdx_, 
                         opfBranch.aFlowFIdx_};
         double cvalaf[] = {opfBranch.y_.real(), opfBranch.y_.imag(), -opfBranch.y_.imag(), 1.0};
         addConstr(4, cindaf, cvalaf, GRB_EQUAL, opfBranch.y_.real());

         int cindar[] = {opfBranch.cosIdx_, opfBranch.opfBus0_->thetaIdx_, opfBranch.opfBus1_->thetaIdx_,
                         opfBranch.aFlowRIdx_};
         double cvalar[] = {opfBranch.y_.real(), -opfBranch.y_.imag(), opfBranch.y_.imag(), 1.0};
         addConstr(4, cindar, cvalar, GRB_EQUAL, opfBranch.y_.real());

         int cindrf[] = {opfBranch.cosIdx_, opfBranch.opfBus0_->thetaIdx_, opfBranch.opfBus1_->thetaIdx_,
                         opfBranch.opfBus0_->phiIdx_, opfBranch.opfBus1_->phiIdx_, opfBranch.rFlowFIdx_};
         double cvalrf[] = {-opfBranch.y_.imag(), opfBranch.y_.real(), -opfBranch.y_.real(), 
            opfBranch.y_.imag(), -opfBranch.y_.imag(), 1.0};
         addConstr(6, cindrf, cvalrf, GRB_EQUAL, -opfBranch.y_.imag());

         int cindrr[] = {opfBranch.cosIdx_, opfBranch.opfBus0_->thetaIdx_, opfBranch.opfBus1_->thetaIdx_,
                         opfBranch.opfBus0_->phiIdx_, opfBranch.opfBus1_->phiIdx_, opfBranch.rFlowRIdx_};
         double cvalrr[] = {-opfBranch.y_.imag(), -opfBranch.y_.real(), opfBranch.y_.real(), -opfBranch.y_.imag(),
                            opfBranch.y_.imag(), 1.0};
         addConstr(6, cindrr, cvalrr, GRB_EQUAL, -opfBranch.y_.imag());
      }

      for (const auto& pair : busses_)
      {
         const OpfBus& opfBus = pair.second;

         int nLines = opfBus.opfBranches_.size();

         std::vector<int> cinda(nLines + 1);
         std::vector<double> cvala(nLines + 1);

         std::vector<int> cindr(nLines + 1);
         std::vector<double> cvalr(nLines + 1);

         int i = 0;

         for (auto elem : opfBus.opfBranches_)
         {
            const auto opfBranch = elem.second;
            OpfBus* fromBus = opfBranch->opfBus0_;
            int activeLineIndex = -1;
            int reactiveLineIndex = -1;
            if (&opfBus == fromBus)
            {
               activeLineIndex = opfBranch->aFlowFIdx_;
               reactiveLineIndex = opfBranch->rFlowFIdx_;
            } 
            else
            {
               activeLineIndex = opfBranch->aFlowRIdx_;
               reactiveLineIndex = opfBranch->rFlowRIdx_;
            }

            cinda[i] = activeLineIndex;
            cvala[i] = 1.0;

            cindr[i] = reactiveLineIndex;
            cvalr[i] = 1.0;

            ++i;
         }

         cinda[i] = opfBus.aGenIdx_;
         cvala[i] = -1.0;

         cindr[i] = opfBus.rGenIdx_;
         cvalr[i] = -1.0;

         // Load constraints.
         addConstr(nLines+1, cinda.data(), cvala.data(), GRB_EQUAL, opfBus.SLoad_.real());
         addConstr(nLines+1, cindr.data(), cvalr.data(), GRB_EQUAL, opfBus.SLoad_.imag());

         if (opfBus.bus_->type() == BusType::SL)
         {
            // Slack bus, theta = 0.
            int cind[] = {opfBus.thetaIdx_};
            double cval[] = {1.0};
            addConstr(1, cind, cval, GRB_EQUAL, 0);
         }

         if (opfBus.bus_->type() == BusType::SL || opfBus.bus_->type() == BusType::PV)
         {
            // Slack or PV bus, phi = |V| - 1.0.
            int cind[] = {opfBus.phiIdx_};
            double cval[] = {1.0};
            addConstr(1, cind, cval, GRB_EQUAL, abs(opfBus.bus_->V()(0)) - 1.0);
         } 
         else
         {
            // PQ bus |V| <= voltageUb_.
            int cind[] = {opfBus.phiIdx_, opfBus.phiSlackUbIdx_};
            double cval[] = {1.0, -1.0};
            addConstr(2, cind, cval, GRB_LESS_EQUAL, voltageUb_ - 1.0);
            //int cind[] = {opfBus.phiIdx_};
            //double cval[] = {1.0};
            //addConstr(1, cind, cval, GRB_LESS_EQUAL, voltageUb_ - 1.0);

            // PQ bus |V| >= voltageLb_.
            int cind2[] = {opfBus.phiIdx_, opfBus.phiSlackLbIdx_};
            double cval2[] = {1.0, 1.0};
            addConstr(2, cind2, cval2, GRB_GREATER_EQUAL, voltageLb_ - 1.0);
            //int cind2[] = {opfBus.phiIdx_};
            //double cval2[] = {1.0};
            //addConstr(1, cind2, cval2, GRB_GREATER_EQUAL, voltageLb_ - 1.0);
         }
      }
      GRBupdatemodel(grbMod_);
   }

   void PowerFlowOpfSolver::retrieveResults()
   {
      for (auto& pair : busses_)
      {
         OpfBus& opfBus = pair.second;

         Bus& bus = *opfBus.bus_;

         double activeVal;
         double reactiveVal;
         double thetaVal;
         double voltageVal;

         GRBgetdblattrelement(grbMod_, "X", opfBus.aGenIdx_, &activeVal);
         GRBgetdblattrelement(grbMod_, "X", opfBus.rGenIdx_, &reactiveVal);
         GRBgetdblattrelement(grbMod_, "X", opfBus.thetaIdx_, &thetaVal);
         GRBgetdblattrelement(grbMod_, "X", opfBus.phiIdx_, &voltageVal);

         voltageVal += 1.0;

         Complex S = {activeVal, reactiveVal};
         Complex V = SmartGridToolbox::polar(voltageVal, thetaVal);
        
         SGT_DEBUG(
               debug() << "SGT name = " << opfBus.bus_->id() << std::endl;
               debug() << "SGT type = " << opfBus.bus_->type() << std::endl;
               debug() << "SLoad    = " << opfBus.SLoad_ << std::endl;
               debug() << "SGen     = " << opfBus.SGen_ << std::endl;
               debug() << "SOL S    = " << S << std::endl;
               debug() << "SOL V    = " << V << std::endl;
               debug() << "SOL |V|  = " << abs(V) << std::endl;
         );

         switch (opfBus.bus_->type())
         {
            case BusType::SL:
               opfBus.bus_->gens()[0]->setSg({S}); // Warm start.
               break;
            case BusType::PQ:
               opfBus.bus_->setV({V}); // Warm start.
               break;
            case BusType::PV:
               opfBus.bus_->setSg({Complex(opfBus.SGen_.real(), S.imag())}); // Warm start.
               opfBus.bus_->setV({V * abs(opfBus.bus_->V()(0)) / abs(V)}); // Warm start.
               break;
            default:
               error() << "Bad bus type." << std::endl;
               SmartGridToolbox::abort();
               break;
         }
         
         opfBus.SSol_ = opfBus.bus_->STot()(0);
         opfBus.VSol_ = V;
      }
   }
   
   int PowerFlowOpfSolver::addVar(const std::string& name, double lb, double ub, double obj)
   {
      int ind[] = {};
      double val[] = {};
      GRBaddvar(grbMod_, 0, ind, val, obj, lb, ub, GRB_CONTINUOUS, name.c_str());
      SGT_DEBUG(debug() << "Added variable " << name << " " << lb << " " << ub << " " << obj << std::endl);
      return nVars_++;
   }
   
   void PowerFlowOpfSolver::addConstr(int nnz, int idxi[], double coeff[], int type, double val)
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
   
   void PowerFlowOpfSolver::postCosineConvexHull(int cindex, int findex, int tindex, double lb, double ub, int steps)
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
   
   void PowerFlowOpfSolver::postSemicircleConvexHull(int aGenIdx, int rGenIdx, double radius, int extraStepsDiv2)
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

   void PowerFlowOpfSolver::printSolution()
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

   void PowerFlowOpfSolver::init(PowerFlowModel* mod)
   {
   }

   bool PowerFlowOpfSolver::solve()
   {
      return false;
   }
}
