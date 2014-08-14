#include "Bus.h"

#include <numeric>
#include <ostream>

namespace SmartGridToolbox
{
   Bus::Bus(const std::string& id, Phases phases, const ublas::vector<Complex>& VNom, double VBase) :
      Component(id),
      phases_(phases),
      VNom_(VNom),
      VBase_(VBase)
   {
      double dtheta = -2.0*pi/phases.size();
      for (int i = 0; i < phases_.size(); ++i)
      {
         VMagSetpoint_(i) = std::abs(VNom_(i));
         VAngSetpoint_(i) = std::arg(VNom_(i));
      }
   }

   ublas::vector<Complex> Bus::YZip() const
   {
      return std::accumulate(zips_.begin(), zips_.end(), ublas::vector<Complex>(phases_.size(), czero),
            [] (ublas::vector<Complex> & tot, const Zip* zip) {return tot + zip->YConst();});
   }

   ublas::vector<Complex> Bus::IZip() const
   {
      return std::accumulate(zips_.begin(), zips_.end(), ublas::vector<Complex>(phases_.size(), czero),
            [] (ublas::vector<Complex> & tot, const Zip* zip) {return tot + zip->IConst();});
   }

   ublas::vector<Complex> Bus::SZip() const
   {
      return std::accumulate(zips_.begin(), zips_.end(), ublas::vector<Complex>(phases_.size(), czero),
            [] (ublas::vector<Complex> & tot, const Zip* zip) {return tot + zip->SConst();});
   }
   
   ublas::vector<Complex> Bus::SGen() const
   {
      return std::accumulate(gens_.begin(), gens_.end(), ublas::vector<Complex>(phases_.size(), czero),
            [] (ublas::vector<Complex> & tot, const Gen* gen) {return tot + gen->S();});
   }

   std::ostream& Bus::print(std::ostream& os) const
   {
      os << "bus:" << std::endl;
      IndentingOStreamBuf _(os);
      os << "id: " << id() << std::endl;
      os << "phases: " << phases_ << std::endl;
      os << "type: " << type_ << std::endl;
      os << "V_base: " << VBase_ << std::endl;
      os << "V_nom: " << VNom_ << std::endl;
      os << "V_mag_min: " << VMagMin_ << std::endl;
      os << "V_mag_max: " << VMagMax_ << std::endl;
      os << "V: " << V_ << std::endl;
      os << "zips:" << std::endl;
      {
         IndentingOStreamBuf _(os);
         for (auto& zip : zips_)
         {
            os << *zip << std::endl;
         }
      }
      os << "gens:" << std::endl;
      {
         IndentingOStreamBuf _(os);
         for (auto& gen : gens_)
         {
            os << *gen << std::endl;
         }
      }
      return os;
   }
}
