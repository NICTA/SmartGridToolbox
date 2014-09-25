#include "CommonBranchParserPlugin.h"

#include "CommonBranch.h"
#include "Network.h"

namespace SmartGridToolbox
{
   void CommonBranchParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      auto cBranch = parseCommonBranch(nd, parser);

      std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
      std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);
      
      netw.addArc(std::move(cBranch), bus0Id, bus1Id);
   }
   
   std::unique_ptr<CommonBranch> CommonBranchParserPlugin::parseCommonBranch(const YAML::Node& nd,
         const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "complex_tap_ratio");
      assertFieldPresent(nd, "Y_series");
      assertFieldPresent(nd, "Y_shunt");

      std::string id = parser.expand<std::string>(nd["id"]);

      std::unique_ptr<CommonBranch> cBranch(new CommonBranch(id));

      cBranch->setTapRatio(parser.expand<Complex>(nd["complex_tap_ratio"]));
      cBranch->setYSeries(parser.expand<Complex>(nd["Y_series"]));
      cBranch->setYShunt(parser.expand<Complex>(nd["Y_shunt"]));

      auto ndRateA =  nd["rate_A"];
      auto ndRateB =  nd["rate_B"];
      auto ndRateC =  nd["rate_C"];

      if (ndRateA)
      {
         cBranch->setRateA(parser.expand<double>(nd["rate_A"]));
      }
      if (ndRateB)
      {
         cBranch->setRateB(parser.expand<double>(nd["rate_B"]));
      }
      if (ndRateC)
      {
         cBranch->setRateC(parser.expand<double>(nd["rate_C"]));
      }

      return cBranch; 
   }
}
