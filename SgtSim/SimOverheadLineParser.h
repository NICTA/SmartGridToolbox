#ifndef SIM_OVERHEAD_LINE_PARSER_DOT_H
#define SIM_OVERHEAD_LINE_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class SimOverheadLine;
   class Simulation;

   /// @brief ParserPlugin that parses OverheadLine objects.
   class SimOverheadLineParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "overhead_line";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
         
         std::unique_ptr<SimOverheadLine> parseSimOverheadLine(const YAML::Node& nd) const;
   };
}

#endif // OVERHEAD_LINE_PARSER_DOT_H
