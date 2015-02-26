#include "SgtNetworkGraph.h"

#include "Branch.h"

#include <vector>

namespace SmartGridToolbox
{
   namespace
   {
      double getXAvg(const BranchAbc& branch)
      {
         int nPhase = branch.phases0().size();
         auto Y = branch.inServiceY();
         double XAvg = 0.0;
         for (int i = 0; i < nPhase; ++i)
         {
            double Xi = -(1.0 / Y(nPhase + i, i)).imag(); 
            XAvg += Xi;
         }
         XAvg /= nPhase;
         return XAvg;
      }

      struct BranchX
      {
         BranchAbc* branch;
         double X;
      };
   }

   void SgtNetworkGraph::create(const Network& nw)
   {
      for (auto bus : nw.busses())
      {
         this->addNode(bus->id(), SgtGraphNodeInfo{0.0, 0.0, 1.0, 1.0, false, 0, bus.get()});
      }

      std::vector<BranchX> branchX; branchX.reserve(nw.branches().size());
      for (auto branch : nw.branches())
      {
         double XAvg = getXAvg(*branch);
         branchX.push_back(BranchX{branch.get(), XAvg});
      }

      double XMax = 0.0;
      for (auto& elem : branchX)
      {
         if (elem.X > XMax)
         {
            XMax = elem.X;
         }
      }
      if (XMax == 0.0)
      {
         XMax = 1.0;
      }

      for (auto& elem : branchX)
      {
         double scaledX = elem.X / XMax;
         this->addArc(elem.branch->id(), elem.branch->bus0()->id(), elem.branch->bus1()->id(),
                      SgtGraphArcInfo{1.0, scaledX, !elem.branch->isInService(), elem.branch});
      }
   };
};
