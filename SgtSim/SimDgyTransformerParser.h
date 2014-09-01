#ifndef DGY_TRANSFORMER_PARSER
#define DGY_TRANSFORMER_PARSER

#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses DgyTransformer objects.
   class DgyTransformerParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "delta_g_wye_transformer";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
         
         std::unique_ptr<SimBranch> parseSimDgyTransformer(const YAML::Node& nd) const;
   };
}

#endif // DGY_TRANSFORMER_PARSER
