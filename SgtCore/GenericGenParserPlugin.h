#ifndef GEN_PARSER_PLUGIN_DOT_H
#define GEN_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace SmartGridToolbox
{
   class GenericGen;
   class Network;

   /// @brief ParserPlugin that parses GenericGen objects.
   class GenericGenParserPlugin : public NetworkParserPlugin
   {
      public:
         virtual const char* key()
         {
            return "generic_gen";
         }

         virtual void parse(const YAML::Node& nd, Network& netw, const ParserState& state) const override;
         
         std::unique_ptr<GenericGen> parseGenericGen(const YAML::Node& nd, const ParserState& state) const;
   };
}

#endif // GEN_PARSER_PLUGIN_DOT_H
