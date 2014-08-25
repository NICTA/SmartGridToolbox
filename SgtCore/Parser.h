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
   using SmartGridToolbox::Time;
   using SmartGridToolbox::posix_time::ptime;
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

   template<> struct convert<Time>
   {
      static Node encode(const Time& from);
      static bool decode(const Node& nd, Time& to);
   };

   template<> struct convert<ptime>
   {
      static Node encode(const ptime& from);
      static bool decode(const Node& nd, ptime& to);
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
   void assertFieldPresent(const YAML::Node& nd, const std::string& field);

   template<typename DataType> class ParserPlugin
   {
      public:

         virtual const char* key() {return "ERROR";}
         virtual void parse(const YAML::Node& nd, DataType& data) const {;}
   };

   YAML::Node getTopNode(const std::string& fname);

   template<typename DataType> class Parser;
   template<typename DataType> void registerParserPlugins(Parser<DataType>& parser);

   template<typename DataType> class Parser {
      public:
         Parser()
         {
            registerParserPlugins(*this);
         }

         void registerParserPlugin(ParserPlugin<DataType> plugin)
         {
            plugins_[plugin.key()] = plugin;
         }
         
         void parse(const std::string& fname, DataType& into)
         {
            message() << "Parsing file " << fname << "." << std::endl;
            {
               IndentingOStreamBuf b(messageStream());
               auto top = getTopNode(fname);
               parse(top, into);
            }
            message() << "Finished parsing file " << fname << "." << std::endl;
         }

         void parse(const YAML::Node& node, DataType& into)
         {
            for (const auto& subnode : node)
            {
               std::string nodeType = subnode.first.as<std::string>();
               const YAML::Node& nodeVal = subnode.second;
               message() << "Parsing plugin " <<  nodeType << "." << std::endl;
               auto it = plugins_.find(nodeType);
               if (it == plugins_.end())
               {
                  warning() << "I don't know how to parse plugin " << nodeType << std::endl;
               }
               else
               {
                  IndentingOStreamBuf b(messageStream());
                  it->second.parse(nodeVal, into);
               }
            }
            message() << "Parsing plugins. Completed." << std::endl;
         }

      private:
         std::map<std::string, ParserPlugin<DataType>> plugins_;
   };
}

#endif // PARSER_DOT_H
