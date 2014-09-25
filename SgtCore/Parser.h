#ifndef PARSER_DOT_H
#define PARSER_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

#include <yaml-cpp/yaml.h>

#include <map>
#include <regex>

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

   // Some no-templated functionality in the base class.
   class ParserBase {
      public:

         template<typename T> T expand(const YAML::Node& nd) const
         {
            return YAML::Node(expandAsString(nd)).as<T>();
         }

      protected:

         std::string expandAsString(const YAML::Node& nd) const;

         struct ParserLoop
         {
            std::string name_;
            int i_;
            std::map<std::string, const YAML::Node*> props_;
         };

         std::vector<ParserLoop> loops_;
   };

   template<typename T> class Parser;

   template<typename T> class ParserPlugin
   {
      public:
         virtual const char* key() {return "ERROR";}
         virtual void parse(const YAML::Node& nd, T& into, const Parser<T>& parser) const 
         {
            // Empty.
         }
         virtual void postParse(const YAML::Node& nd, T& into, const Parser<T>& parser) const
         {
            // Empty.
         }
   };

   YAML::Node getTopNode(const std::string& fname);

   template<typename T> class Parser;
   template<typename T> void registerParserPlugins(Parser<T>& parser);

   template<typename T> class Parser : private ParserBase {
      public:
         Parser()
         {
            registerParserPlugins(*this);
         }

         template<typename PluginType> void registerParserPlugin()
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
               parse(top, into);
            }
            Log().message() << "Finished parsing file " << fname << "." << std::endl;
         }

         void parse(const YAML::Node& node, T& into)
         {
            for (const auto& subnode : node)
            {
               std::string nodeType = subnode.first.as<std::string>();
               const YAML::Node& nodeVal = subnode.second;
               if (nodeType == "loop")
               {
                  const YAML::Node& ndLoopVar = nodeVal["loop_variable"];
                  const YAML::Node& ndLoopProps = nodeVal["loop_properties"];
                  const YAML::Node& ndLoopBody = nodeVal["loop_body"];

                  std::string name = ndLoopVar[0].as<std::string>();
                  int first = ndLoopVar[1].as<int>();
                  int upper = ndLoopVar[2].as<int>();
                  int stride = ndLoopVar[3].as<int>();

                  std::map<std::string, const YAML::Node*> loopProps;
                  if (ndLoopProps)
                  {
                     for (auto nd : ndLoopProps)
                     {
                        std::string id = nd.first.as<std::string>();
                        if (!nd.second.IsSequence())
                        {
                           Log().fatal() << "Lists entries must be a YAML sequence." << std::endl;
                        }
                        auto vec = nd.second.as<std::vector<std::string>>();
                        Log().message() << "phase list " << id << " : " << vec[0] << " ..." << std::endl;
                     }
                  }

                  for (loops_.push_back({name, first, loopProps}); loops_.back().i_ < upper; loops_.back().i_ += stride)
                  {
                     parse(ndLoopBody, into);
                  }
                  loops_.pop_back();
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
                     it->second->parse(nodeVal, into, *this);
                  }
               }
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
