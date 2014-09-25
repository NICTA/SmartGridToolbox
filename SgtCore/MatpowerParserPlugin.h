#ifndef MATPOWER_PARSER_PLUGIN_DOT_H
#define MATPOWER_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace SmartGridToolbox
{
   class Network;

   /// @brief ParserPlugin that parses Matpower files.
   class MatpowerParserPlugin : public NetworkParserPlugin
   {
      public:
         virtual const char* key()
         {
            return "matpower";
         }

         virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;
   };
}

#endif // MATPOWER_PARSER_PLUGIN_DOT_H
