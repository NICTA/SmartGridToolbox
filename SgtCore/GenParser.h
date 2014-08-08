#ifndef GEN_PARSER_DOT_H
#define GEN_PARSER_DOT_H

#include <LibPowerFlow/Parser.h>

namespace LibPowerFlow
{
   /// @brief ParserPlugin that parses Gen objects.
   class GenParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "gen";
         }

      public:
         virtual void parse(const YAML::Node& nd, Network& netw) const override;
   };
}

#endif // GEN_PARSER_DOT_H
