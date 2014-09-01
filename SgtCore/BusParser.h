#ifndef BUS_PARSER_DOT_H
#define BUS_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Bus;
   class Network;

   /// @brief ParserPlugin that parses Bus objects.
   class BusParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "bus";
         }

         virtual void parse(const YAML::Node& nd, Network& into) const override;
         
         std::unique_ptr<Bus> parseBus(const YAML::Node& nd) const;
   };
}

#endif // BUS_PARSER_DOT_H
