#ifndef PARSER_DOT_H
#define PARSER_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

#include <yaml-cpp/yaml.h>
#include <map>

namespace YAML
{
   using SmartGridToolbox::BusType;
   using SmartGridToolbox::Complex;
   using SmartGridToolbox::Phase;
   using SmartGridToolbox::Phases;
   using SmartGridToolbox::ublas::matrix;
   using SmartGridToolbox::ublas::vector;

   template<> struct convert<Complex>
   {
      static Node encode(const Complex& from);
      static bool decode(const Node& nd, Complex& to);
   };

   template<> struct convert<Phase>
   {
      static Node encode(const Phase& from);
      static bool decode(const Node& nd, Phase& to);
   };

   template<> struct convert<Phases>
   {
      static Node encode(const Phases& from);
      static bool decode(const Node& nd, Phases& to);
   };

   template<> struct convert<BusType>
   {
      static Node encode(const BusType& from);
      static bool decode(const Node& nd, BusType& to);
   };

   template<typename T> struct convert<vector<T>>
   {
      static Node encode(const vector<T>& from);
      static bool decode(const Node& nd, vector<T>& to);
   };

   template<typename T> struct convert<matrix<T>>
   {
      static Node encode(const matrix<T>& from);
      static bool decode(const Node& nd, matrix<T>& to);
   };
}

namespace SmartGridToolbox
{
   class Network;

   void assertFieldPresent(const YAML::Node& nd, const std::string& field);

   class ParserPlugin
   {
      public:
         template<typename T> static ParserPlugin& globalCompParser()
         {
            static T t;
            return t;
         }

         static constexpr const char* pluginKey()
         {
            return "component";
         }

      public:
         virtual void parse(const YAML::Node& nd, Network& netw) const {}
   };

   class Parser {
      public:
         static Parser& globalParser()
         {
            static Parser parser;
            return parser;
         };

      public:
         template<typename T> void registerParserPlugin()
         {
            plugins_[T::pluginKey()] = &ParserPlugin::globalCompParser<T>();
         }
         
         const ParserPlugin* plugin(const std::string& name)
         {
            auto it = plugins_.find(name);
            return ((it == plugins_.end()) ? nullptr : it->second);
         }

         void parse(const std::string& fname);
         void parse(const std::string& fname);

      private:
         Parser();

      private:
         std::map<std::string, const ParserPlugin*> plugins_;
   };
}

#endif // PARSER_DOT_H
