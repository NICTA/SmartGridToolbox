#ifndef COMMON_BRANCH_PARSER_DOT_H
#define COMMON_BRANCH_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class CommonBranch;
   class Network;

   /// @brief ParserPlugin that parses CommonBranch objects.
   class CommonBranchParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "common_branch";
         }

         virtual void parse(const YAML::Node& nd, Network& into) const override;
         
         std::unique_ptr<CommonBranch> parseCommonBranch(const YAML::Node& nd) const;
   };
}

#endif // COMMON_BRANCH_PARSER_DOT_H
