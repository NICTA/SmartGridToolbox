#include "GenericZipParserPlugin.h"

#include "Bus.h"
#include "Network.h"
#include "Zip.h"

namespace SmartGridToolbox
{
   void GenericZipParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
   {
      auto zip = parseGenericZip(nd, parser);

      assertFieldPresent(nd, "bus_id");

      std::string busId = parser.expand<std::string>(nd["bus_id"]);
      netw.addZip(std::move(zip), busId);
   }

   std::unique_ptr<GenericZip> GenericZipParserPlugin::parseGenericZip(const YAML::Node& nd, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");

      std::string id = parser.expand<std::string>(nd["id"]);
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
