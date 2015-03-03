#ifndef BRANCH_PARSER_PLUGIN_DOT_H
#define BRANCH_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
   class GenericBranch;
   class Network;

   /// @brief ParserPlugin that parses GenericBranch objects.
   class GenericBranchParserPlugin : public NetworkParserPlugin
   {
      public:
         virtual const char* key()
         {
            return "generic_branch";
         }

         virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;
         
         std::unique_ptr<GenericBranch> parseGenericBranch(const YAML::Node& nd, const ParserBase& parser) const;
   };
}

#endif // BRANCH_PARSER_PLUGIN_DOT_H
