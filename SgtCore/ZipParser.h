#ifndef ZIP_PARSER_DOT_H
#define ZIP_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   /// @brief ParserPlugin that parses Zip objects.
   class Network;
   class ZipParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "zip";
         }

         virtual void parse(const YAML::Node& nd, Network& netw) const override;
   };
}

#endif // ZIP_PARSER_DOT_H
