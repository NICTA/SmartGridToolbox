#ifndef BRANCH_COMP_PARSER_DOT_H
#define BRANCH_COMP_PARSER_DOT_H

#include <SgtSim/Parser.h>

namespace SmartGridToolbox
{
   /// @brief ParserPlugin that parses BranchComp objects.
   class BranchCompParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "branch";
         }

      public:
         virtual void parse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
         virtual void postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
   };
}

#endif // BRANCH_COMP_PARSER_DOT_H
