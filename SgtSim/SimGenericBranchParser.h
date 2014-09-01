#ifndef SIM_GENERIC_BRANCH_PARSER_DOT_H
#define SIM_GENERIC_BRANCH_PARSER_DOT_H

#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses generic branch objects.
   class SimGenericBranchParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key()
         {
            return "sim_generic_branch";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const;
         
         std::unique_ptr<SimBranch> parseSimGenericBranch(const YAML::Node& nd) const;
   };
}

#endif // SIM_GENERIC_BRANCH_PARSER_DOT_H
