#ifndef COMMON_BRANCH_PARSER_DOT_H
#define COMMON_BRANCH_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   /// @brief ParserPlugin that parses CommonBranch objects.
   class Network;
   class CommonBranchParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "common_branch";
         }

         virtual void parse(const YAML::Node& nd, Network& netw) const override;
   };
}

#endif // COMMON_BRANCH_PARSER_DOT_H
