#ifndef BUS_PARSER_PLUGIN_DOT_H
#define BUS_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace Sgt
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

            virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

            std::unique_ptr<Bus> parseBus(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // BUS_PARSER_PLUGIN_DOT_H
