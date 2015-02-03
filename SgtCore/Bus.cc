#include "Bus.h"

#include "Gen.h"
#include "Zip.h"

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

   int Bus::nInServiceGens() const
   {
      int sum = 0;
      for (const auto& elem : genMap_)
      {
         const auto gen = elem.second;
         if (gen->isInService())
         {
            ++sum;
         }
      }
      return sum;
   }

   arma::Col<Complex> Bus::SGen() const
   {
      // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
      // Easier to just do this.
      auto sum = arma::Col<Complex>(phases().size(), arma::fill::zeros);
      for (const auto& elem : genMap_)
      {
         const auto gen = elem.second;
         if (gen->isInService())
         {
            sum += gen->S();
         }
      }
      return sum;
   }

   double Bus::JGen() const
   {
      // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
      // Easier to just do this.
      double sum = 0;
      for (const auto& elem : genMap_)
      {
         const auto gen = elem.second;
         if (gen->isInService())
         {
            sum += gen->J();
         }
      }
      return sum;
   }

   int Bus::nInServiceZips() const
   {
      int sum = 0;
      for (const auto& elem : zipMap_)
      {
         const auto zip = elem.second;
         if (zip->isInService())
         {
            ++sum;
         }
      }
      return sum;
   }

   arma::Col<Complex> Bus::YZip() const
   {
      // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
      // Easier to just do this.
      auto sum = arma::Col<Complex>(phases().size(), arma::fill::zeros);
      for (const auto& elem : zipMap_)
      {
         const auto zip = elem.second;
         sum += zip->YConst();
      }
      return sum;
   }

   arma::Col<Complex> Bus::IZip() const
   {
      // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
      // Easier to just do this.
      auto sum = arma::Col<Complex>(phases().size(), arma::fill::zeros);
      for (const auto& elem : zipMap_)
      {
         const auto zip = elem.second;
         sum += zip->IConst();
      }
      return sum;
   }

   arma::Col<Complex> Bus::SZip() const
   {
      // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
      // Easier to just do this.
      auto sum = arma::Col<Complex>(phases().size(), arma::fill::zeros);
      for (const auto& elem : zipMap_)
      {
         const auto zip = elem.second;
         sum += zip->SConst();
      }
      return sum;
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
