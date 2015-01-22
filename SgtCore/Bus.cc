#include "Bus.h"

#include <ostream>
#include <istream>

namespace
{
   using namespace SmartGridToolbox;
   template<typename T> std::istream& operator>>(std::istringstream& ss, const arma::Col<T>& v)
   {
      return ss;
   }
}

namespace SmartGridToolbox
{
   Bus::Bus(const std::string& id, const Phases& phases, const arma::Col<Complex>& VNom, double VBase) :
      Component(id),
      phases_(phases),
      VNom_(VNom),
      VBase_(VBase),
      VMagSetpoint_(phases.size()),
      VAngSetpoint_(phases.size()),
      V_(VNom)
   {
      for (int i = 0; i < phases_.size(); ++i)
      {
         VMagSetpoint_(i) = std::abs(VNom_(i));
         VAngSetpoint_(i) = std::arg(VNom_(i));
      }
   }

   void Bus::applyVSetpoints()
   {
      arma::Col<Complex> VNew(phases_.size());
      switch (type_)
      {
         case BusType::SL:
            for (int i = 0; i < phases_.size(); ++i)
            {
               VNew(i) = std::polar(VMagSetpoint_(i), VAngSetpoint_(i));
            }
            setV(VNew); // TODO: this triggers an event: is this desirable, or just set V_ directly?
            break;
         case BusType::PV:
            VNew = V_;
            for (int i = 0; i < phases_.size(); ++i)
            {
               VNew(i) *= VMagSetpoint_(i) / std::abs(V_(i));
            }
            setV(VNew);
            break;
         default:
            break;
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
      
   template<> Properties<Bus>::Properties()
   {
      typedef arma::Col<Complex> VType;
      add<const VType&>("V",
            [] (const Bus& bus) -> const VType& {return bus.V();},
            [] (Bus& bus, const VType& V) -> void {bus.setV(V);});
   }
}
