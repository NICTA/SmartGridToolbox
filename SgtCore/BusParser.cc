#include "BusParser.h"

#include "Bus.h"
#include "Common.h"
#include "Network.h"

namespace SmartGridToolbox
{
   void BusParser::parse(const YAML::Node& nd, Network& netw) const
   {
      SGT_DEBUG(debug() << "Bus : parse." << std::endl);
      auto bus = parseBus(nd);
      netw.addNode(std::move(bus));
   }
   
   std::unique_ptr<Bus> BusParser::parseBus(const YAML::Node& nd) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");
      assertFieldPresent(nd, "type");
      assertFieldPresent(nd, "V_base");
      assertFieldPresent(nd, "V_nom");

      std::string id = nd["id"].as<std::string>();
      Phases phases = nd["phases"].as<Phases>();
      BusType type = nd["type"].as<BusType>();
      double VBase = nd["V_base"].as<double>();
      ublas::vector<Complex> VNom = nd["V_nom"].as<ublas::vector<Complex>>();

      std::unique_ptr<Bus> bus(new Bus(id, phases, VNom, VBase));
      bus->setType(type);
      return bus;
   }
}
