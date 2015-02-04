#ifndef POWER_FLOW_OPF_SOLVER_DOT_H
#define POWER_FLOW_OPF_SOLVER_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlowSolver.h>

extern "C" {
#include <gurobi_c.h>
}

#include <map>

namespace SmartGridToolbox
{
   class BranchAbc;
   class Bus;
   class PowerFlowModel;

   struct OpfBranch;

   struct OpfBus
   {
      SmartGridToolbox::Bus* bus_;
      
      std::map<std::string, const OpfBranch*> opfBranches_; // Branches connected to this bus.
      
      Complex SLoad_;
      Complex SGen_;
      double SMagMax_;
      Complex SSol_;
      Complex VSol_;

      int thetaIdx_; // Map from bus name to theta constraint gurobi idx.
      int phiIdx_; // Map from bus name to phi constraint gurobi idx.
      int aGenIdx_; // Map from bus name to active gen constraint gurobi idx.
      int rGenIdx_; // Map from bus name to reactive gen constraint gurobi idx.
      int phiSlackLbIdx_; // Map from bus name to phi slack lb constraint gurobi idx.
      int phiSlackUbIdx_; // Map from bus name to phi slack lb constraint gurobi idx.
   };

   struct OpfBranch
   {
      SmartGridToolbox::BranchAbc* branch_;

      OpfBus* opfBus0_; // From bus.
      OpfBus* opfBus1_; // To bus.

      SmartGridToolbox::Complex y_; // Complex admittance.

      int aFlowFIdx_; // Map from branch name to active forward flow constraint gurobi idx.
      int aFlowRIdx_; // Map from branch name to active backward flow constraint gurobi idx.
      int rFlowFIdx_; // Map from branch name to reactive forward flow constraint gurobi idx.
      int rFlowRIdx_; // Map from branch name to reactive backward flow constraint gurobi idx.
      int cosIdx_; // Map from branch name to cos constraint gurobi idx.
   };

   class PowerFlowOpfSolver : public PowerFlowSolverInterface
   {
      public:
         virtual void init(PowerFlowModel* mod) override;
         virtual bool solve() override;
      private:
         void addBus(SmartGridToolbox::Bus& bus);
         void addBranch(SmartGridToolbox::BranchAbc& branch);
         void linkBussesAndBranches();
         void recreateBusData();
         void recreateBranchData();
         void createGrbModel();
         void retrieveResults();
         int addVar(const std::string & name, double lb, double ub, double obj);
         void addConstr(int nnz, int idxi[], double coeff[], int type, double val);
         void postCosineConvexHull(int cindex, int findex, int tindex, double lb, double ub, int steps);
         void postSemicircleConvexHull(int aGenIdx, int rGenIdx, double radius, int extraStepsDiv2);
         void printSolution();
      private:
         double voltageLb_;
         double voltageUb_;
         std::map<std::string, OpfBus> busses_;
         std::map<std::string, OpfBranch> branches_;
         GRBenv * grbEnv_;
         GRBmodel * grbMod_;
         int nVars_;
         int iter_;
   };
};

#endif // POWER_FLOW_OPF_SOLVER_ABC_DOT_H
