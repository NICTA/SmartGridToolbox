#include "ZipParser.h"

#include "Bus.h"
#include "Network.h"
#include "Zip.h"

namespace SmartGridToolbox
{
   void ZipParser::parse(const YAML::Node& nd, Network& netw) const
   {
      SGT_DEBUG(debug() << "Zip : parse." << std::endl);

      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "Y_const");
      assertFieldPresent(nd, "I_const");
      assertFieldPresent(nd, "S_const");
      assertFieldPresent(nd, "bus_id");

      std::string id = nd["id"].as<std::string>();
      ublas::vector<Complex> Y = nd["Y_const"].as<ublas::vector<Complex>>();
      ublas::vector<Complex> I = nd["I_const"].as<ublas::vector<Complex>>();
      ublas::vector<Complex> S = nd["S_const"].as<ublas::vector<Complex>>();
      std::string busId = nd["bus_id"].as<std::string>();
      
      Bus* bus = netw.bus(busId);

      std::unique_ptr<Zip> zip(new Zip(id, bus->phases()));
      zip->setYConst(Y);
      zip->setIConst(I);
      zip->setSConst(S);

      netw.addZip(std::move(zip), *bus);
   }
}
