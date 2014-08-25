#include "DgyTransformerParser.h"

#include "DgyTransformer.h"
#include "Network.h"

namespace SmartGridToolbox
{
   void DgyTransformerParser::parse(const YAML::Node& nd, Network& netw) const
   {
      SGT_DEBUG(debug() << "DgyTransformer : parse." << std::endl);

      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "complex_turns_ratio_01");
      assertFieldPresent(nd, "leakage_impedance");
      assertFieldPresent(nd, "bus_0");
      assertFieldPresent(nd, "bus_1");

      const std::string id = nd["id"].as<std::string>();
      Complex a = nd["complex_turns_ratio_01"].as<Complex>();
      Complex ZL = nd["leakage_impedance"].as<Complex>();
      std::string bus0Id = nd["bus_0_id"].as<std::string>();
      std::string bus1Id = nd["bus_1_id"].as<std::string>();

      std::unique_ptr<DgyTransformer> trans(new DgyTransformer(id, a, ZL));

      std::shared_ptr<Bus> bus0 = netw.bus(bus0Id);
      assert(bus0 != nullptr);
      std::shared_ptr<Bus> bus1 = netw.bus(bus1Id);
      assert(bus1 != nullptr);
      trans->setBus0(bus0);
      trans->setBus1(bus1);
      
      netw.addBranch(std::move(trans));
   }
}
