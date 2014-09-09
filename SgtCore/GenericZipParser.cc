#include "GenericZipParser.h"

#include "Bus.h"
#include "Network.h"
#include "Zip.h"

namespace SmartGridToolbox
{
   void GenericZipParser::parse(const YAML::Node& nd, Network& into) const
   {
      SGT_DEBUG(debug() << "GenericZip : parse." << std::endl);

      auto zip = parseGenericZip(nd);

      assertFieldPresent(nd, "bus_id");

      std::string busId = nd["bus_id"].as<std::string>();
      into.addZip(std::move(zip), busId);
   }

   std::unique_ptr<GenericZip> GenericZipParser::parseGenericZip(const YAML::Node& nd) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");

      std::string id = nd["id"].as<std::string>();
      Phases phases = nd["phases"].as<Phases>();

      std::unique_ptr<GenericZip> zip(new GenericZip(id, phases));

      auto ndYConst = nd["Y_const"];
      auto ndIConst = nd["I_const"];
      auto ndSConst = nd["S_const"];

      if (ndYConst)
      {
         zip->setYConst(nd["Y_const"].as<ublas::vector<Complex>>());
      }
      if (ndIConst)
      {
         zip->setIConst(nd["I_const"].as<ublas::vector<Complex>>());
      }
      if (ndSConst)
      {
         zip->setSConst(nd["S_const"].as<ublas::vector<Complex>>());
      }

      return zip;
   }

}
