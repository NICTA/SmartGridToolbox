#ifndef BUS_PARSER_DOT_H
#define BUS_PARSER_DOT_H

#include <SgtCore/NetworkParser.h>

namespace SmartGridToolbox
{
   class Bus;
   class Network;

   /// @brief ParserPlugin that parses Bus objects.
   class BusParserPlugin : public NetworkParserPlugin
   {
      public:
         virtual const char* key()
         {
            return "bus";
         }

         virtual void parse(const YAML::Node& nd, Network& netw, const ParserState& state) const override;
         
         std::unique_ptr<Bus> parseBus(const YAML::Node& nd, const ParserState& state) const;
   };
}

#endif // BUS_PARSER_DOT_H
