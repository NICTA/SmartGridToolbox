#ifndef ZIP_PARSER_DOT_H
#define ZIP_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Network;
   class Zip;

   /// @brief ParserPlugin that parses Zip objects.
   class ZipParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "zip";
         }

         virtual void parse(const YAML::Node& nd, Network& into) const override;
         
         std::unique_ptr<Zip> parseZip(const YAML::Node& nd) const;
   };
}

#endif // ZIP_PARSER_DOT_H
