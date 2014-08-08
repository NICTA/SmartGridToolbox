#ifndef COMMON_BRANCH_PARSER_DOT_H
#define COMMON_BRANCH_PARSER_DOT_H

#include <LibPowerFlow/Parser.h>

namespace LibPowerFlow
{
   /// @brief ParserPlugin that parses CommonBranch objects.
   class CommonBranchParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "common_branch";
         }

      public:
         virtual void parse(const YAML::Node& nd, Network& netw) const override;
   };
}

#endif // COMMON_BRANCH_PARSER_DOT_H
