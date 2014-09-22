#include "CommonBranchParser.h"

#include "CommonBranch.h"
#include "Network.h"

namespace SmartGridToolbox
{
   void CommonBranchParser::parse(const YAML::Node& nd, Network& netw, const ParserState& state) const
   {
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      auto cBranch = parseCommonBranch(nd, state);

      std::string bus0Id = state.expandName(nd["bus_0_id"].as<std::string>());
      std::string bus1Id = state.expandName(nd["bus_1_id"].as<std::string>());
      
      netw.addArc(std::move(cBranch), bus0Id, bus1Id);
   }
   
   std::unique_ptr<CommonBranch> CommonBranchParser::parseCommonBranch(const YAML::Node& nd,
         const ParserState& state) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "complex_tap_ratio");
      assertFieldPresent(nd, "Y_series");
      assertFieldPresent(nd, "Y_shunt");

      std::string id = state.expandName(nd["id"].as<std::string>());

      std::unique_ptr<CommonBranch> cBranch(new CommonBranch(id));

      cBranch->setTapRatio(nd["complex_tap_ratio"].as<Complex>());
      cBranch->setYSeries(nd["Y_series"].as<Complex>());
      cBranch->setYShunt(nd["Y_shunt"].as<Complex>());

      auto ndRateA =  nd["rate_A"];
      auto ndRateB =  nd["rate_B"];
      auto ndRateC =  nd["rate_C"];

      if (ndRateA)
      {
         cBranch->setRateA(nd["rate_A"].as<double>());
      }
      if (ndRateB)
      {
         cBranch->setRateB(nd["rate_B"].as<double>());
      }
      if (ndRateC)
      {
         cBranch->setRateC(nd["rate_C"].as<double>());
      }

      return cBranch; 
   }
}
