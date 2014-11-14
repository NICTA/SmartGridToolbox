#ifndef YAML_SUPPORT_DOT_H
#define YAML_SUPPORT_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

#include <yaml-cpp/yaml.h>

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

   Emitter& operator<<(YAML::Emitter& out, const Complex& c)
   {
      out << YAML::Flow << SmartGridToolbox::to_string(c);
      return out;
   }

   template<typename T> YAML::Emitter& operator<<(YAML::Emitter& out, const boost::numeric::ublas::vector<T>& v) {
      out << YAML::Flow;
      out << YAML::BeginSeq;
      for (auto x : v)
      {
         out << x;
      }
      out << YAML::EndSeq;
      return out;
   }
}

namespace SmartGridToolbox
{
   template<typename T> std::string to_string(const T& t)
   {
      YAML::Emitter e;
      return (e << t).c_str();
   }

   template<typename T> (const T& t)

   {
      YAML::Emitter e;
      return (e << t).c_str();
   }
}

#endif // YAML_SUPPORT_DOT_H
