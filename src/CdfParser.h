#ifndef CDF_PARSER_DOT_H
#define CDF_PARSER_DOT_H

#include <SmartGridToolbox/Parser.h>

namespace SmartGridToolbox
{
   /// ParserPlugin that parses IEEE CDF data files.
   /** A network object, named according to the "network_name" yaml field,  will be added to the model. The network
    ** will by populated with branches and busses named {network_name}_bus_{bus_id}, etc. */
   class CdfParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "cdf";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };
}

#endif // CDF_PARSER_DOT_H
