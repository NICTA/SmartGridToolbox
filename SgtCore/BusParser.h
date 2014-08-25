#ifndef BUS_PARSER_DOT_H
#define BUS_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   /// @brief ParserPlugin that parses Bus objects.
   class Network;
   class BusParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "bus";
         }

         virtual void parse(const YAML::Node& nd, Network& netw) const override;
   };
}

#endif // BUS_PARSER_DOT_H
