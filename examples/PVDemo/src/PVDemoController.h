#ifndef PV_DEMO_CONTROLLER_DOT_H
#define PV_DEMO_CONTROLLER_DOT_H

#include "PVDemoInverter.h"

#include <SmartGridToolbox/InverterBase.h>
#include <SmartGridToolbox/Network.h>

extern "C" {
#include <gurobi_c.h>
}

namespace PVDemo
{
   struct PVDemoBus;
   struct PVDemoBranch;

   struct PVDemoBus
   {
      SmartGridToolbox::Bus * sgtBus_;
      
      std::set<const PVDemoBranch *> pvdBranches_;     // Branches connected to this bus.
      
      Complex SLoad_;
      Complex SGen_;
      double SMagMax_;
      Complex SSol_;
      Complex VSol_;

      PVDemoInverter * specialInverter_;

      int thetaIdx_;                                   // Map from bus name to theta constraint gurobi idx.
      int phiIdx_;                                     // Map from bus name to phi constraint gurobi idx.
      int aGenIdx_;                                    // Map from bus name to active gen constraint gurobi idx.
      int rGenIdx_;                                    // Map from bus name to reactive gen constraint gurobi idx.
      int phiSlackLbIdx_;                              // Map from bus name to phi slack lb constraint gurobi idx.
      int phiSlackUbIdx_;                              // Map from bus name to phi slack lb constraint gurobi idx.
   };

   struct PVDemoBranch
   {
      SmartGridToolbox::Branch * sgtBranch_;

      PVDemoBus * pvdBus0_;                  // From bus.
      PVDemoBus * pvdBus1_;                  // To bus.

      SmartGridToolbox::Complex y_;          // Complex admittance.

      int aFlowFIdx_;                        // Map from branch name to active forward flow constraint gurobi idx.
      int aFlowRIdx_;                        // Map from branch name to active backward flow constraint gurobi idx.
      int rFlowFIdx_;                        // Map from branch name to reactive forward flow constraint gurobi idx.
      int rFlowRIdx_;                        // Map from branch name to reactive backward flow constraint gurobi idx.
      int cosIdx_;                           // Map from branch name to cos constraint gurobi idx.
   };

   class PVDemoController : public SmartGridToolbox::Component
   {
      public:
         PVDemoController(const std::string & name, SmartGridToolbox::Network & network,
                          double voltageLb, double voltageUb);
         virtual ~PVDemoController() {}
         std::vector<const PVDemoBus *> busses();
         std::vector<const PVDemoBranch *> branches();

      private:
         virtual void initializeState() override;
         virtual void updateState(SmartGridToolbox::Time t0, SmartGridToolbox::Time t1) override;

      private:
         void addBus(SmartGridToolbox::Bus & bus);
         void addBranch(SmartGridToolbox::Branch & branch);
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
         SmartGridToolbox::Network * network_;
         double voltageLb_;
         double voltageUb_;

         std::map<std::string, PVDemoBus> busses_;
         std::map<std::string, PVDemoBranch> branches_;
         GRBenv * grbEnv_;
         GRBmodel * grbMod_;
         int nVars_;
         int iter_;
   };
};

#endif // PV_DEMO_CONTROLLER_DOT_H
