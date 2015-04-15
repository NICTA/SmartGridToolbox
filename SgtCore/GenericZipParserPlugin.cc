#include "GenericZipParserPlugin.h"

#include "Bus.h"
#include "Network.h"
#include "Zip.h"
#include "YamlSupport.h"

namespace Sgt
{
   void GenericZipParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
   {
      auto zip = parseGenericZip(nd, parser);

      assertFieldPresent(nd, "bus_id");

      std::string busId = parser.expand<std::string>(nd["bus_id"]);
      netw.addZip(std::move(zip), busId);
   }

   std::unique_ptr<GenericZip> GenericZipParserPlugin::parseGenericZip(const YAML::Node& nd,
         const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");

      std::string id = parser.expand<std::string>(nd["id"]);
      Phases phases = parser.expand<Phases>(nd["phases"]);

      std::unique_ptr<GenericZip> zip(new GenericZip(id, phases));

      auto ndYConst = nd["Y_const"];
      auto ndIConst = nd["I_const"];
      auto ndSConst = nd["S_const"];

      if (ndYConst)
      {
         zip->setYConst(parser.expand<arma::Col<Complex>>(nd["Y_const"]));
      }
      if (ndIConst)
      {
         zip->setIConst(parser.expand<arma::Col<Complex>>(nd["I_const"]));
      }
      if (ndSConst)
      {
         zip->setSConst(parser.expand<arma::Col<Complex>>(nd["S_const"]));
      }

      return zip;
   }

}
