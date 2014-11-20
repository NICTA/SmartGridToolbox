#include "Zip.h"

#include <ostream>

namespace SmartGridToolbox
{
   GenericZip::GenericZip(const std::string& id, const Phases& phases) :
      ZipAbc(id, phases),
      YConst_(phases.size(), arma::fill::zeros),
      IConst_(phases.size(), arma::fill::zeros),
      SConst_(phases.size(), arma::fill::zeros)
   {
      // Empty.
   }

   void ZipAbc::print(std::ostream& os) const
   {
      Component::print(os);
      StreamIndent _(os);
      os << "id: " << id() << std::endl;
      os << "phases: " << phases() << std::endl;
      os << "YConst: " << YConst() << std::endl;
      os << "IConst: " << IConst() << std::endl;
      os << "SConst: " << SConst() << std::endl;
   }
}
