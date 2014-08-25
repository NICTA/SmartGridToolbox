#include "CommonBranchParser.h"

#include "CommonBranch.h"
#include "Network.h"

namespace SmartGridToolbox
{
   void CommonBranchParser::parse(const YAML::Node& nd, Network& netw) const
   {
      SGT_DEBUG(debug() << "CommonBranch : parse." << std::endl);

      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "complex_tap_ratio");
      assertFieldPresent(nd, "Y_series");
      assertFieldPresent(nd, "Y_shunt");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      std::string id = nd["id"].as<std::string>();

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

      std::string bus0Id = nd["bus_0_id"].as<std::string>();
      std::string bus1Id = nd["bus_1_id"].as<std::string>();
      
      std::shared_ptr<Bus> bus0 = netw.bus(bus0Id);
      assert(bus0 != nullptr);
      std::shared_ptr<Bus> bus1 = netw.bus(bus1Id);
      assert(bus1 != nullptr);

      cBranch->setBus0(bus0);
      cBranch->setBus1(bus1);
      
      netw.addBranch(std::move(cBranch));

   }
}
