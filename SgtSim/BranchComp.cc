#include <SgtSim/BranchComp.h>
#include <SgtSim/BusComp.h>
#include <SgtSim/Model.h>
#include <SgtSim/NetworkComp.h>
#include <SgtSim/PowerFlow.h>

namespace SmartGridToolbox
{
   BranchComp::BranchComp(const std::string& name, const Phases& phases0, const Phases& phases1) :
      Component(name),
      bus0_(nullptr),
      bus1_(nullptr),
      phases0_(phases0),
      phases1_(phases1),
      Y_(2*phases0.size(), 2*phases0.size(), czero),
      changed_("BranchComp " + name + " setpoint changed")
   {
      if (phases0.size() != phases1.size())
      {
         error() << "BranchComp must have equal number of phases for both sides." << std::endl;
         abort();
      }
   }

   void BranchComp::setBus0(BusComp& bus0)
   {
      if (!phases0_.isSubsetOf(bus0.phases()))
      {
         error() << "Connecting a bus to a branch with a phase mismatch." << std::endl;
         abort();
      }
      bus0_ = &bus0;
      changed().trigger();
   }

   void BranchComp::setBus1(BusComp& bus1)
   {
      if (!phases1_.isSubsetOf(bus1.phases()))
      {
         error() << "Connecting a bus to a branch with a phase mismatch." << std::endl;
         abort();
      }
      bus1_ = &bus1;
      changed().trigger();
   }

   void BranchComp::setY(const ublas::matrix<Complex>& Y)
   {
      if (Y.size1() != 2*phases0_.size() && Y.size2() != 2*phases0_.size())
      {
         error() << "Both dimensions of Y should be of size twice the number of phases." << std::endl;
         abort();
      }
      Y_ = Y;
      changed().trigger();
   }
}
