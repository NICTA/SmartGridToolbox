#ifndef COMMON_BRANCH_PARSER_PLUGIN_DOT_H
#define COMMON_BRANCH_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
   class CommonBranch;
   class Network;

   /// @brief ParserPlugin that parses CommonBranch objects.
   class CommonBranchParserPlugin : public NetworkParserPlugin
   {
      public:
         virtual const char* key()
         {
            return "common_branch";
         }

         virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

         std::unique_ptr<CommonBranch> parseCommonBranch(const YAML::Node& nd, const ParserBase& parser) const;
   };
}

#endif // COMMON_BRANCH_PARSER_PLUGIN_DOT_H
