#include "Bus.h"

#include <ostream>

namespace SmartGridToolbox
{
   Bus::Bus(const std::string& id, const Phases& phases, const ublas::vector<Complex>& VNom, double VBase) :
      Component(id),
      phases_(phases),
      VNom_(VNom),
      VBase_(VBase)
   {
      double dtheta = -2.0 * pi / phases.size();
      for (int i = 0; i < phases_.size(); ++i)
      {
         VMagSetpoint_(i) = std::abs(VNom_(i));
         VAngSetpoint_(i) = std::arg(VNom_(i));
      }
   }

   void Bus::print(std::ostream& os) const
   {
      Component::print(os);
      StreamIndent _(os);
      os << "phases: " << phases() << std::endl;
      os << "type: " << type() << std::endl;
      os << "V_base: " << VBase() << std::endl;
      os << "V_nom: " << VNom() << std::endl;
      os << "V_mag_min: " << VMagMin() << std::endl;
      os << "V_mag_max: " << VMagMax() << std::endl;
      os << "V: " << V() << std::endl;
   }
}
