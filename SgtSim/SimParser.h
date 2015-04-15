#ifndef SIM_PARSER_DOT_H
#define SIM_PARSER_DOT_H

#include<SgtCore/Parser.h>

namespace Sgt
{
    class Simulation;
    extern template class Parser<Simulation>;
    extern template class ParserPlugin<Simulation>;
    using SimParser = Parser<Simulation>;
    using SimParserPlugin = ParserPlugin<Simulation>;
}

#endif // SIM_PARSER_DOT_H
