#ifndef MATPOWER_PARSER_DOT_H
#define MATPOWER_PARSER_DOT_H

#include <SmartGridToolbox/Parser.h>

namespace SmartGridToolbox
{
   /// @brief ParserPlugin that parses Matpower data files.
   ///
   /// A network object, named according to the "network_name" yaml field,  will be added to the model. The network
   /// will by populated with branches and busses named {network_name}_bus_{bus_id}, etc.
   class MatpowerParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "matpower";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };
}

#endif // MATPOWER_PARSER_DOT_H
