#ifndef UNDERGROUND_LINE_PARSER_PLUGIN
#define UNDERGROUND_LINE_PARSER_PLUGIN

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
   class Network;
   class UndergroundLine;

   /// @brief ParserPlugin that parses UndergroundLine objects.
   class UndergroundLineParserPlugin : public NetworkParserPlugin
   {
      public:
         virtual const char* key()
         {
            return "underground_line";
         }

         virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

         std::unique_ptr<UndergroundLine> parseUndergroundLine(const YAML::Node& nd, const ParserBase& parser) const;
   };
}

#endif // UNDERGROUND_LINE_PARSER_PLUGIN
