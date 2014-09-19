#include "GenericZipParser.h"

#include "Bus.h"
#include "Network.h"
#include "Zip.h"

namespace SmartGridToolbox
{
   void GenericZipParser::parse(const YAML::Node& nd, Network& netw, const ParserState& state) const
   {
      auto zip = parseGenericZip(nd, state);

      assertFieldPresent(nd, "bus_id");

      std::string busId = state.expandName(nd["bus_id"].as<std::string>());
      netw.addZip(std::move(zip), busId);
   }

   std::unique_ptr<GenericZip> GenericZipParser::parseGenericZip(const YAML::Node& nd, const ParserState& state) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");

      std::string id = state.expandName(nd["id"].as<std::string>());
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
