#include <SmartGridToolbox/Branch.h>
#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/Model.h>
#include <SmartGridToolbox/Network.h>
#include <SmartGridToolbox/PowerFlow.h>
 
namespace SmartGridToolbox
{
   void BranchParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Branch : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus_0");
      assertFieldPresent(nd, "bus_1");
      assertFieldPresent(nd, "phases_0");
      assertFieldPresent(nd, "phases_1");
      assertFieldPresent(nd, "Y");

      string name = state.expandName(nd["name"].as<std::string>());
      Phases phases0 = nd["phases_0"].as<Phases>();
      Phases phases1 = nd["phases_1"].as<Phases>();

      const YAML::Node & ndY = nd["Y"];
      const YAML::Node & ndYMatrix = ndY["matrix"];
      const YAML::Node & ndYSimpleLine = ndY["simple_line"];
      ublas::matrix<Complex> Y; 
      if (ndYMatrix)
      {
         Y = ndYMatrix.as<ublas::matrix<Complex>>();
      }
      else if (ndYSimpleLine)
      {
         ublas::vector<Complex> y = ndYSimpleLine.as<ublas::vector<Complex>>();
         Y = YSimpleLine(y); 
      }

      Branch & comp = mod.newComponent<Branch>(name, phases0, phases1);
      comp.setY(Y);
   }

   void BranchParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Branch : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      Branch * comp = mod.componentNamed<Branch>(name);

      const std::string networkStr = state.expandName(nd["network"].as<std::string>());
      Network * networkComp = mod.componentNamed<Network>(networkStr);
      if (networkComp != nullptr)
      {
         networkComp->addBranch(*comp);
      }
      else
      {
         error() << "For component " << name <<  ", network " << networkStr 
                 <<  " was not found in the model." << std::endl;
         abort();
      }

      // Depends on network freq, so do in postParse.
      const YAML::Node & ndY = nd["Y"];
      const YAML::Node & ndYOverhead = ndY["overhead_line"];
      if (ndYOverhead)
      {
         ublas::vector<double> r = ndYOverhead["r"].as<ublas::vector<double>>();
         ublas::matrix<double> D = ndYOverhead["D"].as<ublas::matrix<double>>();
         double length = ndYOverhead["length"].as<double>();
         const YAML::Node & ndEarthResist = ndYOverhead["earth_resist"];
         double rho = ndEarthResist ? ndEarthResist.as<double>() : 100.0;
         comp->setY(YOverheadLine(r, D, length, networkComp->freq(), rho));
      }

      const std::string bus0Str = state.expandName(nd["bus_0"].as<std::string>());
      Bus * bus0Comp = mod.componentNamed<Bus>(bus0Str);
      if (networkComp != nullptr)
      {
         comp->setBus0(*bus0Comp);
      }
      else
      {
         error() << "For component " << name <<  ", bus " << bus0Str 
                 <<  " was not found in the model." << std::endl;
         abort();
      }

      const std::string bus1Str = state.expandName(nd["bus_1"].as<std::string>());
      Bus * bus1Comp = mod.componentNamed<Bus>(bus1Str);
      if (networkComp != nullptr)
      {
         comp->setBus1(*bus1Comp);
      }
      else
      {
         error() << "For component " << name <<  ", bus " << bus1Str 
                 <<  " was not found in the model." << std::endl;
         abort();
      }
   }

   Branch::Branch(const std::string & name, const Phases & phases0, const Phases & phases1) :
      Component(name),
      bus0_(nullptr),
      bus1_(nullptr),
      phases0_(phases0),
      phases1_(phases1),
      Y_(2 * phases0.size(), 2 * phases0.size(), czero)
   {
      if (phases0.size() != phases1.size())
      {
         error() << "Branch must have equal number of phases for both sides." << std::endl;
         abort();
      }
   }

   void Branch::setBus0(Bus & bus0) 
   {
      if (!phases0_.isSubsetOf(bus0_->phases()))
      {
         error() << "Connecting a bus to a branch with a phase mismatch." << std::endl;
         abort();
      }
      bus0_ = &bus0; needsUpdate().trigger();
   }
   
   void Branch::setBus1(Bus & bus1) 
   {
      if (!phases1_.isSubsetOf(bus1_->phases()))
      {
         error() << "Connecting a bus to a branch with a phase mismatch." << std::endl;
         abort();
      }
      bus1_ = &bus1; needsUpdate().trigger();
   }

   void Branch::setY(const ublas::matrix<Complex> & Y)
   {
      if (Y.size1() != 2 * phases0_.size() && Y.size2() != 2 * phases0_.size())
      {
         error() << "Both dimensions of Y should be of size twice the number of phases." << std::endl;
         abort();
      }
      Y_ = Y;
      needsUpdate().trigger();
   }
}
