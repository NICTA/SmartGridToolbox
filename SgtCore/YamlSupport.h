#ifndef YAML_SUPPORT_DOT_H
#define YAML_SUPPORT_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

#include <yaml-cpp/yaml.h>

namespace SmartGridToolbox
{
   template<typename T> std::string toYamlString(const T& t)
   {
      YAML::Emitter e;
      return (e << t).c_str();
   }
   
   inline std::string toYamlString(const Complex& c)
   {
      return to_string(c);
   }
   
   template<typename T> T fromYamlString(const std::string& s)
   {
      return YAML::Load(s).as<T>();
   }
}

namespace YAML
{
   using SmartGridToolbox::BusType;
   using SmartGridToolbox::Complex;
   using SmartGridToolbox::Phase;
   using SmartGridToolbox::Phases;
   using SmartGridToolbox::Time;
   using SmartGridToolbox::posix_time::ptime;

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

   template<typename T> struct convert<arma::Col<T>>
   {
      static Node encode(const arma::Col<T>& from);
      static bool decode(const Node& nd, arma::Col<T>& to);
   };

   template<typename T> struct convert<arma::Mat<T>>
   {
      static Node encode(const arma::Mat<T>& from);
      static bool decode(const Node& nd, arma::Mat<T>& to);
   };

   Emitter& operator<<(Emitter& out, const Complex& c);

   template<typename T> Emitter& operator<<(Emitter& out, const arma::Col<T>& v) 
   {
      out << YAML::Flow;
      out << YAML::BeginSeq;
      for (size_t i = 0; i < v().size(); ++i)
      {
         out << v()(i);
      }
      out << YAML::EndSeq;
      return out;
   }
}

#endif // YAML_SUPPORT_DOT_H
