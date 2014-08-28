#include "ZipParser.h"

#include "Bus.h"
#include "Network.h"
#include "Zip.h"

namespace SmartGridToolbox
{
   void ZipParser::parse(const YAML::Node& nd, Network& netw) const
   {
      SGT_DEBUG(debug() << "Zip : parse." << std::endl);

      auto zip = parseZip(nd);

      assertFieldPresent(nd, "bus_id");

      std::string busId = nd["bus_id"].as<std::string>();
      const std::shared_ptr<Bus>& bus = netw.bus(busId);
      netw.addZip(std::move(zip), *bus);
   }

   std::unique_ptr<Zip> ZipParser::parseZip(const YAML::Node& nd) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");
      assertFieldPresent(nd, "Y_const");
      assertFieldPresent(nd, "I_const");
      assertFieldPresent(nd, "S_const");

      std::string id = nd["id"].as<std::string>();
      Phases phases = nd["phases"].as<Phases>();
      ublas::vector<Complex> Y = nd["Y_const"].as<ublas::vector<Complex>>();
      ublas::vector<Complex> I = nd["I_const"].as<ublas::vector<Complex>>();
      ublas::vector<Complex> S = nd["S_const"].as<ublas::vector<Complex>>();
      
      std::unique_ptr<Zip> zip(new Zip(id, phases));
      zip->setYConst(Y);
      zip->setIConst(I);
      zip->setSConst(S);

      return zip;
   }

}
