#include "Network.h"

namespace SmartGridToolbox
{
   Network::Network(const std::string& id, Model& model, double PBase) :
      Component(id),
      model_(&model),
      PBase_(PBase)
   {
      // Empty.
   }

   void Network::print(std::ostream& os) const
   {
      Component::print(os);
      IndentingOStreamBuf _(os);
      os << "P_base: " << PBase_ << std::endl;
      for (auto& bus : busVec_)
      {
         os << *bus << std::endl;
      }
      for (auto& branch : branchVec_)
      {
         os << *branch << std::endl;
      }
   }
}
