#ifndef MATPOWER_PARSER_DOT_H
#define MATPOWER_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   /// @brief ParserPlugin that parses Matpower files.
   class Network;
   class MatpowerParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "matpower";
         }

         virtual void parse(const YAML::Node& nd, Network& netw) const override;
   };
}

#endif // MATPOWER_PARSER_DOT_H
