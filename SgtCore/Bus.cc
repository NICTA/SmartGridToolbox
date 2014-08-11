#include "Bus.h"

#include <numeric>
#include <ostream>

namespace SmartGridToolbox
{
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
      IndentingOStreamBuf ind(os, "");
      os << "bus:" << std::endl;
      ind.setInd("    ");
      os << "id: " << id_ << std::endl;
      os << "phases: " << phases_ << std::endl;
      os << "type: " << type_ << std::endl;
      os << "V_base: " << VBase_ << std::endl;
      os << "V_nom: " << VNom_ << std::endl;
      os << "V_mag_min: " << VMagMin_ << std::endl;
      os << "V_mag_max: " << VMagMax_ << std::endl;
      os << "V: " << V_ << std::endl;
      os << "zips:" << std::endl;
      ind.setInd("        ");
      for (auto& zip : zips_)
      {
         os << *zip << std::endl;
      }
      ind.setInd("    ");
      os << "gens:" << std::endl;
      ind.setInd("        ");
      for (auto& gen : gens_)
      {
         os << *gen << std::endl;
      }
      return os;
   }
}
