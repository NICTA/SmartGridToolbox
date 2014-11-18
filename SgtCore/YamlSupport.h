#ifndef YAML_SUPPORT_DOT_H
#define YAML_SUPPORT_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

#include <yaml-cpp/yaml.h>

#include <iostream>

namespace SmartGridToolbox
{
   template<typename T> std::string toYamlString(const T& t)
   {
      std::cout << "toYamlString" << std::endl;
      YAML::Emitter e;
      return (e << t).c_str();
   }
   
   inline std::string toYamlString(const Complex& c)
   {
      std::cout << "toYamlString c" << std::endl;
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
   using SmartGridToolbox::ublas::matrix;
   using SmartGridToolbox::ublas::matrix_expression;
   using SmartGridToolbox::ublas::vector;
   using SmartGridToolbox::ublas::vector_expression;

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

   template<typename VE> struct convert<vector_expression<VE>>
   {
      static Node encode(const vector_expression<VE>& from)
      {
         Node nd;
         for (const auto& x : from()) nd.push_back(x);
         return nd;
      }
   };

   template<typename ME> struct convert<matrix_expression<ME>>
   {
      Node encode(const matrix_expression<ME>& from)
      {
         Node nd;
         for (int i = 0; i < from().size1(); ++i)
         {
            Node nd1;
            for (int k = 0; k < from().size2(); ++k)
            {
               nd1.push_back(from()(i, k));
            }
            nd.push_back(nd1);
         }
         return nd;
      }
   };

   Emitter& operator<<(Emitter& out, const Complex& c);

   template<typename VE> Emitter& operator<<(Emitter& out, const vector_expression<VE>& v) 
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
