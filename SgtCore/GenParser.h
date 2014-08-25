#ifndef GEN_PARSER_DOT_H
#define GEN_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   /// @brief ParserPlugin that parses Gen objects.
   class Network;
   class GenParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "gen";
         }

         virtual void parse(const YAML::Node& nd, Network& netw) const override;
   };
}

#endif // GEN_PARSER_DOT_H
