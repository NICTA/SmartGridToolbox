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

      struct BranchTempData
      {
         BranchAbc* branch;
         double X;
         bool hasTrans;
      };
   }

   void SgtNetworkGraph::create(const Network& nw)
   {
      for (auto bus : nw.busses())
      {
         this->addNode(bus->id(), SgtGraphNodeInfo{0.0, 0.0, 1.0, 1.0, false, 0, bus.get()});
      }

      std::vector<BranchTempData> branchTempData; branchTempData.reserve(nw.branches().size());
      for (auto branch : nw.branches())
      {
         double XAvg = getXAvg(*branch);

         bool hasTrans = false;
         auto Y = branch->inServiceY();
         if (std::abs(branch->bus0()->VBase() - branch->bus1()->VBase()) > 1e-6)
         {
            hasTrans = true;
            std::cout << branch->id() << " has a transformer A" << std::endl;
         }
         else
         {
            for (int i = 0; i < branch->phases0().size(); ++i)
            {
               for (int j = 0; j < i; ++j)
               {
                  if (std::abs(Y(i, j) - Y(j, i)) > 1e-6)
                  {
                     hasTrans = true;
                     std::cout << branch->id() << " has a transformer B" << std::endl;
                     goto endLoop;
                  }
               }
            }
endLoop:;
         }

         branchTempData.push_back(BranchTempData{branch.get(), XAvg, hasTrans});
      }

      double XMax = 0.0;
      for (auto& elem : branchTempData)
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

      for (auto& elem : branchTempData)
      {
         double scaledX = elem.X / XMax;
         this->addArc(elem.branch->id(), elem.branch->bus0()->id(), elem.branch->bus1()->id(),
                      SgtGraphArcInfo{1.0, scaledX, elem.hasTrans, !elem.branch->isInService(), elem.branch});
      }

   };
};
