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
         virtual const char* key() override
         {
            return "generic_branch";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim) const override;
   };
}

#endif // SIM_GENERIC_BRANCH_PARSER_DOT_H
