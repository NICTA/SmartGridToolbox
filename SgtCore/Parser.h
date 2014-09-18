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
   class ParserState
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
   void assertFieldPresent(const YAML::Node& nd, const std::string& field);

   template<typename T> class ParserPlugin
   {
      public:

         virtual const char* key() {return "ERROR";}
         virtual void parse(const YAML::Node& nd, T& into, const ParserState& state) const 
         {
            // Empty.
         }
         virtual void postParse(const YAML::Node& nd, T& into, const ParserState& state) const
         {
            // Empty.
         }
   };

   YAML::Node getTopNode(const std::string& fname);

   template<typename T> class Parser;
   template<typename T> void registerParserPlugins(Parser<T>& parser);

   template<typename T> class Parser {
      public:
         Parser()
         {
            registerParserPlugins(*this);
         }

         template<class PluginType> void registerParserPlugin()
         {
            auto plugin = std::unique_ptr<PluginType>(new PluginType());
            plugins_[plugin->key()] = std::move(plugin);
         }
         
         void parse(const std::string& fname, T& into)
         {
            Log().message() << "Parsing file " << fname << "." << std::endl;
            {
               Indent _;
               auto top = getTopNode(fname);
               ParserState state;
               parse(top, into, state);
            }
            Log().message() << "Finished parsing file " << fname << "." << std::endl;
         }

         void parse(const YAML::Node& node, T& into, ParserState& state)
         {
            for (const auto& subnode : node)
            {
               std::string nodeType = subnode.first.as<std::string>();
               const YAML::Node& nodeVal = subnode.second;
               if (nodeType == "loop")
               {
                  std::string name = nodeVal["name"].as<std::string>();
                  int count = nodeVal["count"].as<int>();
                  const YAML::Node & body = nodeVal["body"];
                  for (state.pushLoop(name); state.topLoopVal() < count; state.incrTopLoop())
                  {
                     parse(body, into, state);
                  }
                  state.popLoop();
               }
               else
               {
                  Log().message() << "Parsing plugin " <<  nodeType << "." << std::endl;
                  auto it = plugins_.find(nodeType);
                  if (it == plugins_.end())
                  {
                     Log().warning() << "I don't know how to parse plugin " << nodeType << std::endl;
                  }
                  else
                  {
                     Indent _;
                     it->second->parse(nodeVal, into);
                  }
               }
               Log().message() << "Parsing completed." << std::endl;
            }
         }

      private:
         std::map<std::string, std::unique_ptr<ParserPlugin<T>>> plugins_;
   };

   class Network;
   extern template class Parser<Network>;
   extern template class ParserPlugin<Network>;
}

#endif // PARSER_DOT_H
