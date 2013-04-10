#ifndef PARSER_DOT_H
#define PARSER_DOT_H

#include "Common.h"
#include "Model.h"
#include "../third_party/yaml-cpp-0.5.0/include/yaml-cpp/yaml.h"

namespace SmartGridToolbox
{
   class Parser {
      public:
         void Parse(const char * fname, Model & model);
      private:
         void ParseGlobal(const YAML::Node & top, Model & model);
         void ParseObjects(const YAML::Node & top, Model & model,
                           bool isPrototype);
   };
}

#endif // PARSER_DOT_H
