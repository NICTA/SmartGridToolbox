#ifndef PARSER_DOT_H
#define PARSER_DOT_H

#include "Common.h"
#include "../third_party/yaml-cpp-0.5.0/include/yaml-cpp/yaml.h"
#include <list>

namespace SmartGridToolbox
{
   class Component;
   class Model;
   class Simulation;

   class ComponentParser
   {
      public:
         virtual void parse(const YAML::Node & comps, Model & mod) = 0;
   };

   class Parser {
      public:
         static Parser & getGlobalParser()
         {
            static Parser parser;
            return parser;
         };

      public:
         void parse(const char * fname, Model & model,
                    Simulation & simulation);
      private:
         Parser() = default;

         void registerComponentParser(const ComponentParser & compParser);

         void parseGlobal(const YAML::Node & top, Model & model,
                          Simulation & simulation);
         void parseComponents(const YAML::Node & top, Model & model,
                              bool isPrototype);

      private:
         std::list<ComponentParser *> compParsers_;
   };
}

#endif // PARSER_DOT_H
