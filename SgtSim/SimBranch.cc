#include "SimBranch.h"

#include "SimBus.h"
#include "Model.h"
#include "SimNetwork.h"

namespace SmartGridToolbox
{
   SimBranch::SimBranch(const std::string& name, const Phases& phases0, const Phases& phases1) :
      Component(name),
      bus0_(nullptr),
      bus1_(nullptr),
      phases0_(phases0),
      phases1_(phases1),
      Y_(2*phases0.size(), 2*phases0.size(), czero),
      changed_("SimBranch " + name + " setpoint changed")
   {
      if (phases0.size() != phases1.size())
      {
         error() << "SimBranch must have equal number of phases for both sides." << std::endl;
         abort();
      }
   }

   void SimBranch::setBus0(SimBus& bus0)
   {
      if (!phases0_.isSubsetOf(bus0.phases()))
      {
         error() << "Connecting a bus to a branch with a phase mismatch." << std::endl;
         abort();
      }
      bus0_ = &bus0;
      changed().trigger();
   }

   void SimBranch::setBus1(SimBus& bus1)
   {
      if (!phases1_.isSubsetOf(bus1.phases()))
      {
         error() << "Connecting a bus to a branch with a phase mismatch." << std::endl;
         abort();
      }
      bus1_ = &bus1;
      changed().trigger();
   }

   void SimBranch::setY(const ublas::matrix<Complex>& Y)
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
