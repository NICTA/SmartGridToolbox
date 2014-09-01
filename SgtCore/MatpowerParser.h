#ifndef MATPOWER_PARSER_DOT_H
#define MATPOWER_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Network;

   /// @brief ParserPlugin that parses Matpower files.
   class MatpowerParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "matpower";
         }

         virtual void parse(const YAML::Node& nd, Network& into) const override;
   };
}

#endif // MATPOWER_PARSER_DOT_H
