#ifndef BRANCH_PARSER_DOT_H
#define BRANCH_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Network;

   /// @brief ParserPlugin that parses GenericBranch objects.
   class GenericBranchParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "generic_branch";
         }

         virtual void parse(const YAML::Node& nd, Network& netw) const override;
   };
}

#endif // BRANCH_PARSER_DOT_H
