#ifndef SIM_YY_TRANSFORMER_PARSER
#define SIM_YY_TRANSFORMER_PARSER

#include <SgtSim/SimNetworkComponent.h>

#include <SgtSim/SimParser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses YyTransformer objects.
   class SimYyTransformerParserPlugin : public SimParserPlugin
   {
      public:
         virtual const char* key() override
         {
            return "yy_transformer";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const override;
   };
}

#endif // SIM_YY_TRANSFORMER_PARSER
