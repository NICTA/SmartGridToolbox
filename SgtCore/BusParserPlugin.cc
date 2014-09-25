#include "BusParserPlugin.h"

#include "Bus.h"
#include "Network.h"

namespace SmartGridToolbox
{
   void BusParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
   {
      auto bus = parseBus(nd, parser);
      netw.addNode(std::move(bus));
   }
   
   std::unique_ptr<Bus> BusParserPlugin::parseBus(const YAML::Node& nd, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");
      assertFieldPresent(nd, "type");
      assertFieldPresent(nd, "V_base");
      assertFieldPresent(nd, "V_nom");

      std::string id = parser.expand<std::string>(nd["id"]);
      Phases phases = parser.expand<Phases>(nd["phases"]);
      BusType type = parser.expand<BusType>(nd["type"]);
      double VBase = parser.expand<double>(nd["V_base"]);
      ublas::vector<Complex> VNom = parser.expand<ublas::vector<Complex>>(nd["V_nom"]);

      std::unique_ptr<Bus> bus(new Bus(id, phases, VNom, VBase));
      bus->setType(type);
      return bus;
   }
}
