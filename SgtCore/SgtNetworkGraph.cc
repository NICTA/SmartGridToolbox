#include "SgtNetworkGraph.h"

#include "Branch.h"

#include <vector>

namespace SmartGridToolbox
{
   namespace
   {
      double getRAvg(const BranchAbc& branch)
      {
         int nPhase = branch.phases0().size();
         auto Y = branch.Y();
         double rAvg = 0.0;
         for (int i = 0; i < nPhase; ++i)
         {
            rAvg -= Y(nPhase + i, i).real(); 
         }
         rAvg /= nPhase;
         return rAvg;
      }

      struct BranchR
      {
         BranchAbc* branch;
         double r;
      };
   }

   void SgtNetworkGraph::create(const Network& nw)
   {
      for (auto bus : nw.busses())
      {
         this->addNode(bus->id(), SgtGraphNodeInfo{0.0, 0.0, 1.0, 1.0, bus.get()});
      }

      std::vector<BranchR> branchR; branchR.reserve(nw.branches().size());
      for (auto branch : nw.branches())
      {
         double rAvg = getRAvg(*branch);
         branchR.push_back(BranchR{branch.get(), rAvg});
      }
      double rMax = 0.0;
      for (auto& elem : branchR)
      {
         if (elem.r > rMax)
         {
            rMax = elem.r;
         }
      }
      const double lMin = 2;
      const double lMax = 10;
      for (auto& elem : branchR)
      {
         double l = std::max(lMin, lMax * elem.r / rMax);
         this->addArc(elem.branch->id(), elem.branch->bus0()->id(), elem.branch->bus1()->id(),
                      SgtGraphArcInfo{l, elem.branch});
      }
   };
};
