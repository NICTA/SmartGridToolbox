#ifndef GEN_PARSER_DOT_H
#define GEN_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Gen;
   class Network;

   /// @brief ParserPlugin that parses Gen objects.
   class GenParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "gen";
         }

         virtual void parse(const YAML::Node& nd, Network& into) const override;
         
         std::unique_ptr<Gen> parseGen(const YAML::Node& nd) const;
   };
}

#endif // GEN_PARSER_DOT_H
