#ifndef SIMPLE_BUILDING_PARSER_DOT_H
#define SIMPLE_BUILDING_PARSER_DOT_H

#include <SmartGridToolbox/Parser.h>

namespace SmartGridToolbox
{
   class SimpleBuildingParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "simple_building";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };
}

#endif // SIMPLE_BUILDING_PARSER_DOT_H
