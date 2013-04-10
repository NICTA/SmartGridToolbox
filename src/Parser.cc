#include "Parser.h"

namespace SmartGridToolbox
{
   Parser::Parse(const char * fname)
   {
      std::ifstream fin("test.yaml");
      YAML::Parser parser(fin);

      YAML::Node doc;
      while(parser.GetNextDocument(doc)) {
         // ...
      }
   }
}
