#ifndef ZIP_COMP_PARSER_DOT_H
#define ZIP_COMP_PARSER_DOT_H

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class SimZipParser : public ParserPlugin<Simulation>
   {
      class Simulation;

      public:
         virtual const char* key() override
         {
            return "zip_to_ground";
         }
      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
   };
}

#endif // ZIP_COMP_PARSER_DOT_H
