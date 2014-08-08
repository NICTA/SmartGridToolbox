#include "GenParser.h"

#include "Bus.h"
#include "Gen.h"
#include "Network.h"

namespace LibPowerFlow
{
   void GenParser::parse(const YAML::Node& nd, Network& netw) const
   {
      LPF_DEBUG(debug() << "Gen : parse." << std::endl);

      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "bus_id");

      std::string id = nd["id"].as<std::string>();
      std::string busId = nd["bus_id"].as<std::string>();
      Phases phases = nd["phases"].as<Phases>();

      Bus* bus = netw.bus(busId);

      std::unique_ptr<Gen> gen(new Gen(id, bus->phases()));

      if (const YAML::Node& subNd = nd["S"])
      {
         gen->setS(subNd.as<ublas::vector<Complex>>());
      }
      
      if (const YAML::Node& subNd = nd["P_min"])
      {
         gen->setPMin(subNd.as<double>());
      }

      if (const YAML::Node& subNd = nd["P_max"])
      {
         gen->setPMax(subNd.as<double>());
      }
      
      if (const YAML::Node& subNd = nd["Q_min"])
      {
         gen->setQMin(subNd.as<double>());
      }

      if (const YAML::Node& subNd = nd["Q_max"])
      {
         gen->setQMax(subNd.as<double>());
      }
      
      if (const YAML::Node& subNd = nd["C0"])
      {
         gen->setC0(subNd.as<double>());
      }
      
      if (const YAML::Node& subNd = nd["C1"])
      {
         gen->setC1(subNd.as<double>());
      }
      
      if (const YAML::Node& subNd = nd["C2"])
      {
         gen->setC2(subNd.as<double>());
      }
      
      netw.addGen(std::move(gen), *bus);
   }
}
