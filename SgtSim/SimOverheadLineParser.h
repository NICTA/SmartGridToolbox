#ifndef SIM_OVERHEAD_LINE_PARSER_DOT_H
#define SIM_OVERHEAD_LINE_PARSER_DOT_H

#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses OverheadLine objects.
   class SimOverheadLineParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key()
         {
            return "overhead_line";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const;
         
         std::unique_ptr<SimBranch> parseSimOverheadLine(const YAML::Node& nd) const;
   };
}

#endif // OVERHEAD_LINE_PARSER_DOT_H
