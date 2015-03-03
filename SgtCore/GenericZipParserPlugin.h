#ifndef ZIP_PARSER_PLUGIN_DOT_H
#define ZIP_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
   class Network;
   class GenericZip;

   /// @brief ParserPlugin that parses GenericZip objects.
   class GenericZipParserPlugin : public NetworkParserPlugin
   {
      public:
         virtual const char* key()
         {
            return "generic_zip";
         }

         virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;
         
         std::unique_ptr<GenericZip> parseGenericZip(const YAML::Node& nd, const ParserBase& parser) const;
   };
}

#endif // ZIP_PARSER_PLUGIN_DOT_H
