#ifndef PARSER_DOT_H
#define PARSER_DOT_H

#include "Common.h"
#include "../third_party/yaml-cpp-0.5.0/include/yaml-cpp/yaml.h"
#include <map>

namespace SmartGridToolbox
{
   class Component;
   class ComponentParser;
   class Model;
   class Simulation;

   class ComponentParser
   {
      public:
         template<typename T> static ComponentParser & getGlobalCompParser()
         {
            static T t;
            return t; 
         }

         virtual void parse(const YAML::Node & comp, Model & mod) const = 0;
   };

   class Parser {

      friend class ComponentParser;

      public:
         static Parser & getGlobalParser()
         {
            static Parser parser;
            return parser;
         };

      public:
         void parse(const char * fname, Model & model,
                    Simulation & simulation);

         template<typename T>
         void registerComponentParser()
         {
            compParsers_[T::getComponentName()] = 
               &ComponentParser::getGlobalCompParser<T>();
         }

         const ComponentParser * getComponentParser(const std::string & name)
         {
            auto it = compParsers_.find(name);
            return ((it == compParsers_.end()) ? nullptr : it->second);
         }

      private:
         Parser() = default;

         void parseGlobal(const YAML::Node & top, Model & model,
                          Simulation & simulation);

         void parseComponents(const YAML::Node & top, Model & model,
                              bool isPrototype);

      private:
         std::map<std::string, const ComponentParser *> compParsers_;
   };

}

#endif // PARSER_DOT_H
