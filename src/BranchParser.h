#ifndef BRANCH_PARSER_DOT_H
#define BRANCH_PARSER_DOT_H

#include <SmartGridToolbox/Parser.h>

namespace SmartGridToolbox
{
   class BranchParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "branch";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };
}

#endif // BRANCH_PARSER_DOT_H
