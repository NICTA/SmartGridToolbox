#ifndef PARSER_DOT_H
#define PARSER_DOT_H

#include "Common.h"
#include "../third_party/yaml-cpp-0.5.0/include/yaml-cpp/yaml.h"
#include <map>

namespace YAML
{
   using SmartGridToolbox::UblasVector;
   using SmartGridToolbox::UblasMatrix;
   using SmartGridToolbox::Complex;

   template<> struct convert<UblasVector<double>>
   {
      static Node encode(const UblasVector<double> & from);
      static bool decode(const Node& nd, UblasVector<double> & to);
   };

   template<> struct convert<UblasVector<Complex>>
   {
      static Node encode(const UblasVector<Complex> & from); 
      static bool decode(const Node& nd, UblasVector<Complex> & to);
   };

   template<> struct convert<UblasMatrix<double>>
   {
      static Node encode(const UblasMatrix<double> & from);
      static bool decode(const Node& nd, UblasMatrix<double> & to);
   };

   template<> struct convert<UblasMatrix<Complex>>
   {
      static Node encode(const UblasMatrix<Complex> & from);
      static bool decode(const Node& nd, UblasMatrix<Complex> & to);
   };
}

namespace SmartGridToolbox
{
   class Component;
   class ComponentParser;
   class Model;
   class Simulation;

   inline void assertFieldPresent(const YAML::Node & nd, const std::string & field)
   {
      if (!(nd[field]))
      {
         error("Parsing bus_1_phase: \"%s\" field not present.", field.c_str());
      }
   }

   inline Complex parseComplex(const std::string & s)
   {
      return complexFromString(s);
   }

   UblasVector<double> parseDoubleVector(const YAML::Node & nd);
   UblasMatrix<Complex> parseDoubleMatrix(const YAML::Node & nd);
   UblasVector<double> parseComplexVector(const YAML::Node & nd);
   UblasMatrix<Complex> parseComplexMatrix(const YAML::Node & nd);

   class ComponentParser
   {
      public:
         template<typename T> static ComponentParser & getGlobalCompParser()
         {
            static T t;
            return t; 
         }

         static constexpr const char * getComponentName() 
         {
            return "component";
         }


      public:
         virtual void parse(const YAML::Node & comp, Model & mod) const = 0;
         virtual void postParse(const YAML::Node & comp, Model & mod) const = 0;
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

         template<typename T> void registerComponentParser()
         {
            compParsers_[T::getComponentName()] = &ComponentParser::getGlobalCompParser<T>();
            // Note: Could also be implemented using type_info. However, the getComponentName() function is 
            // useful elsewhere, and this is probably simpler.
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
