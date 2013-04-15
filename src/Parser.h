#ifndef PARSER_DOT_H
#define PARSER_DOT_H

#include "Common.h"
#include "Model.h"
#include "../third_party/yaml-cpp-0.5.0/include/yaml-cpp/yaml.h"

namespace SmartGridToolbox
{
   class Simulation;

   class Parser {
      public:
         void Parse(const char * fname, Model & model,
                    Simulation & simulation);
      private:
         void ParseGlobal(const YAML::Node & top, Model & model,
                          Simulation & simulation);
         void ParseObjects(const YAML::Node & top, Model & model,
                           bool isPrototype);
   };
}

#endif // PARSER_DOT_H
