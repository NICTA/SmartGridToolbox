#ifndef GEN_PARSER_DOT_H
#define GEN_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class GenericGen;
   class Network;

   /// @brief ParserPlugin that parses GenericGen objects.
   class GenericGenParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "generic_gen";
         }

         virtual void parse(const YAML::Node& nd, Network& into) const override;
         
         std::unique_ptr<GenericGen> parseGenericGen(const YAML::Node& nd) const;
   };
}

#endif // GEN_PARSER_DOT_H
