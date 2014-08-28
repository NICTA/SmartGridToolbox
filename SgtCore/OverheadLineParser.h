#ifndef OVERHEAD_LINE_PARSER
#define OVERHEAD_LINE_PARSER

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Network;
   
   /// @brief ParserPlugin that parses OverheadLine objects.
   class OverheadLineParser : public ParserPlugin<Network>
   {
      public:
         virtual const char* key()
         {
            return "overhead_line";
         }

         virtual void parse(const YAML::Node& nd, Network& netw) const override;
   };
}

#endif // OVERHEAD_LINE_PARSER
