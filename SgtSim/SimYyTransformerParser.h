#ifndef SIM_YY_TRANSFORMER_PARSER
#define SIM_YY_TRANSFORMER_PARSER

#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses YyTransformer objects.
   class SimYyTransformerParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "sim_yy_transformer";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
   };
}

#endif // SIM_YY_TRANSFORMER_PARSER
