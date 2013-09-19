#ifndef PARSER_DOT_H
#define PARSER_DOT_H

#include "Common.h"
#include "Model.h"
#include "PowerFlow.h"
#include "../third_party/yaml-cpp-0.5.0/include/yaml-cpp/yaml.h"
#include <map>

namespace YAML
{
   using SmartGridToolbox::BusType;
   using SmartGridToolbox::Complex;
   using SmartGridToolbox::Phase;
   using SmartGridToolbox::Phases;
   using SmartGridToolbox::Time;
   using SmartGridToolbox::ptime;
   using SmartGridToolbox::UblasMatrix;
   using SmartGridToolbox::UblasVector;

   template<> struct convert<Complex>
   {
      static Node encode(const Complex & from);
      static bool decode(const Node & nd, Complex & to);
   };

   template<> struct convert<Phase>
   {
      static Node encode(const Phase & from);
      static bool decode(const Node & nd, Phase & to);
   };

   template<> struct convert<Phases>
   {
      static Node encode(const Phases & from);
      static bool decode(const Node & nd, Phases & to);
   };

   template<> struct convert<BusType>
   {
      static Node encode(const BusType & from);
      static bool decode(const Node & nd, BusType & to);
   };

   template<> struct convert<Time>
   {
      static Node encode(const Time & from);
      static bool decode(const Node & nd, Time & to);
   };

   template<> struct convert<ptime>
   {
      static Node encode(const ptime & from);
      static bool decode(const Node & nd, ptime & to);
   };

   template<typename T> struct convert<UblasVector<T>>
   {
      static Node encode(const UblasVector<T> & from);
      static bool decode(const Node & nd, UblasVector<T> & to);
   };

   template<typename T> struct convert<UblasMatrix<T>>
   {
      static Node encode(const UblasMatrix<T> & from);
      static bool decode(const Node & nd, UblasMatrix<T> & to);
   };
}

namespace SmartGridToolbox
{
   class Component;
   class ParserPlugin;
   class Model;
   class Simulation;

   struct ParserState
   {
      public:
         std::string expandName(const std::string & target) const;

         void pushLoop(const std::string & name)
         {
            loops_.push_back({name, 0});
         }

         int topLoopVal()
         {
            return loops_.back().i_;
         }

         void incrTopLoop()
         {
            ++loops_.back().i_;
         }

         void popLoop()
         {
            loops_.pop_back();
         }

      private:
         struct ParserLoop
         {
            std::string name_;
            int i_;
         };

      private:
         std::vector<ParserLoop> loops_;
   };

   void assertFieldPresent(const YAML::Node & nd, const std::string & field);

   class ParserPlugin
   {
      public:
         template<typename T> static ParserPlugin & globalCompParser()
         {
            static T t;
            return t; 
         }

         static constexpr const char * pluginKey() 
         {
            return "component";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const {}
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const {}
   };

   class Parser {

      friend class ParserPlugin;

      public:
         static Parser & globalParser()
         {
            static Parser parser;
            return parser;
         };

      public:
         void parse(const char * fname, Model & model, Simulation & simulation);

         template<typename T> void registerParserPlugin()
         {
            compParsers_[T::pluginKey()] = &ParserPlugin::globalCompParser<T>();
            // Note: Could also be implemented using type_info. However, the pluginKey() function is 
            // useful elsewhere, and this is probably simpler.
         }

         const ParserPlugin * componentParser(const std::string & name)
         {
            auto it = compParsers_.find(name);
            return ((it == compParsers_.end()) ? nullptr : it->second);
         }

         Time parseTime(const YAML::Node & nd, const Model & model)
         {
            return timeFromLocalTime(nd.as<ptime>(), model.timezone());
         }

      private:
         Parser();

         void parseGlobal(const YAML::Node & top, Model & model,
                          Simulation & simulation);

         void parseComponents(const YAML::Node & node, ParserState & state, Model & model, bool isPostParse);

      private:
         std::map<std::string, const ParserPlugin *> compParsers_;
   };

}

#endif // PARSER_DOT_H
