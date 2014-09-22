#ifndef SIM_PARSER_DOT_H
#define SIM_PARSER_DOT_H

#include<SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;
   extern template class Parser<Simulation>;
   extern template class ParserPlugin<Simulation>;
   using SimParser = Parser<Simulation>;
   using SimParser = ParserPlugin<Simulation>;
}

#endif // SIM_PARSER_DOT_H
