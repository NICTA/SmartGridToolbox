#ifndef ZIP_PARSER_DOT_H
#define ZIP_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Network;
   class GenericZip;

   /// @brief ParserPlugin that parses GenericZip objects.
   class GenericZipParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "generic_zip";
         }

         virtual void parse(const YAML::Node& nd, Network& into) const override;
         
         std::unique_ptr<GenericZip> parseGenericZip(const YAML::Node& nd) const;
   };
}

#endif // ZIP_PARSER_DOT_H
