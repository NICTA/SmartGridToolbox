#include "GenericGenParserPlugin.h"

#include "Bus.h"
#include "Gen.h"
#include "Network.h"

namespace SmartGridToolbox
{
   void GenericGenParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserState& state) const
   {
      auto gen = parseGenericGen(nd, state);

      assertFieldPresent(nd, "bus_id");
      std::string busId = state.expandName(nd["bus_id"].as<std::string>());
      netw.addGen(std::move(gen), busId);
   }
   
   std::unique_ptr<GenericGen> GenericGenParserPlugin::parseGenericGen(const YAML::Node& nd,
         const ParserState& state) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");

      std::string id = state.expandName(nd["id"].as<std::string>());
      Phases phases = nd["phases"].as<Phases>();

      std::unique_ptr<GenericGen> gen(new GenericGen(id, phases));

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

      return gen;
   }
}
