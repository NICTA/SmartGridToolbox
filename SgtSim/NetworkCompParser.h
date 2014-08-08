#ifndef NETWORK_COMP_PARSER_DOT_H
#define NETWORK_COMP_PARSER_DOT_H

#include <SgtSim/Parser.h>

namespace SmartGridToolbox
{
   /// @brief ParserPlugin that parses the network keyword, adding a new NetworkComp to the model.
   class NetworkCompParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "network";
         }

      public:
         virtual void parse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
   };
}

#endif // NETWORK_COMP_PARSER_DOT_H
