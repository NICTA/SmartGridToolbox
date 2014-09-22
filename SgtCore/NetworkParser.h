#ifndef NETWORK_PARSER_DOT_H
#define NETWORK_PARSER_DOT_H

#include<SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Network;
   extern template class Parser<Network>;
   extern template class ParserPlugin<Network>;
   using NetworkParser = Parser<Network>;
   using NetworkParser = ParserPlugin<Network>;
}

#endif // NETWORK_PARSER_DOT_H
