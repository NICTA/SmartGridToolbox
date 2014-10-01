#include "GenericGenParserPlugin.h"

#include "Bus.h"
#include "Gen.h"
#include "Network.h"

namespace SmartGridToolbox
{
   void GenericGenParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
   {
      auto gen = parseGenericGen(nd, parser);

      assertFieldPresent(nd, "bus_id");
      std::string busId = parser.expand<std::string>(nd["bus_id"]);
      netw.addGen(std::move(gen), busId);
   }
   
   std::unique_ptr<GenericGen> GenericGenParserPlugin::parseGenericGen(const YAML::Node& nd,
         const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");

      std::string id = parser.expand<std::string>(nd["id"]);
      Phases phases = parser.expand<Phases>(nd["phases"]);

      std::unique_ptr<GenericGen> gen(new GenericGen(id, phases));

      if (const YAML::Node& subNd = nd["S"])
      {
         gen->setS(parser.expand<ublas::vector<Complex>>(subNd));
      }
      
      if (const YAML::Node& subNd = nd["P_min"])
      {
         gen->setPMin(parser.expand<double>(subNd));
      }

      if (const YAML::Node& subNd = nd["P_max"])
      {
         gen->setPMax(parser.expand<double>(subNd));
      }
      
      if (const YAML::Node& subNd = nd["Q_min"])
      {
         gen->setQMin(parser.expand<double>(subNd));
      }

      if (const YAML::Node& subNd = nd["Q_max"])
      {
         gen->setQMax(parser.expand<double>(subNd));
      }
      
      if (const YAML::Node& subNd = nd["C0"])
      {
         gen->setC0(parser.expand<double>(subNd));
      }
      
      if (const YAML::Node& subNd = nd["C1"])
      {
         gen->setC1(parser.expand<double>(subNd));
      }
      
      if (const YAML::Node& subNd = nd["C2"])
      {
         gen->setC2(parser.expand<double>(subNd));
      }

      return gen;
   }
}
