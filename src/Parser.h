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

   // TODO: implement these as template magic rather than repeated code?

   template<> struct convert<UblasVector<double>>
   {
      static Node encode(const UblasVector<double> & from) {
         Node nd;
         for (double val : from) nd.push_back(val);
         return nd;
      }

      static bool decode(const Node& nd, UblasVector<double> & to)
      {
         if(!nd.IsSequence())
         {
            return false;
         }
         else
         {
            int sz = nd.size();
            to = UblasVector<double>(sz);
            for (int i = 0; i < sz; ++i)
            {
               to(i) = nd[i].as<double>();
            }
         }
         return true;
      }
   };

   template<> struct convert<UblasVector<Complex>>
   {
      static Node encode(const UblasVector<Complex> & from) {
         Node nd;
         for (Complex val : from)
         {
            Node nd1;
            nd1.push_back(val.real());
            nd1.push_back(val.imag());
            nd.push_back(nd1);
         }
         return nd;
      }

      static bool decode(const Node& nd, UblasVector<Complex> & to)
      {
         if(!nd.IsSequence())
         {
            return false;
         }
         else
         {
            int sz = nd.size();
            to = UblasVector<Complex>(sz);
            for (int i = 0; i < sz; ++i)
            {
               double re = nd[0].as<double>();
               double im = nd[1].as<double>();
               to(i) = Complex(re, im);
            }
         }
         return true;
      }
   };

   template<> struct convert<UblasMatrix<double>>
   {
      static Node encode(const UblasMatrix<double> & from) {
         Node nd;
         for (int i = 0; i < from.size1(); ++i)
         {
            Node nd1;
            for (int k = 0; k < from.size2(); ++k)
            {
               nd1.push_back(from(i, k));
            }
            nd.push_back(nd1);
         }
         return nd;
      }

      static bool decode(const Node& nd, UblasMatrix<double> & to)
      {
         if(!nd.IsSequence())
         {
            return false;
         }
         else
         {
            int nrows = nd.size();
            if (nrows == 0)
            {
               std::cerr << "Matrix has zero rows in yaml." << std::endl;
               return false;
            }
            int ncols = nd[0].size();
            if (ncols == 0)
            {
               std::cerr << "Matrix has zero columns in yaml." << std::endl;
               return false;
            }
            for (int i = 1; i < nrows; ++i)
            {
               if (nd[i].size() != ncols)
               {
                  std::cerr << "Ill-formed matrix in yaml." << std::endl;
                  return false;
               }
            }
            to = UblasMatrix<double>(nrows, ncols);
            for (int i = 0; i < nrows; ++i)
            {
               for (int k = 0; k < nrows; ++k)
               {
                  to(i, k) = nd[i][k].as<double>();
               }
            }
         }
         return true;
      }
   };

   template<> struct convert<UblasMatrix<Complex>>
   {
      static Node encode(const UblasMatrix<Complex> & from) {
         Node nd;
         for (int i = 0; i < from.size1(); ++i)
         {
            Node nd1;
            for (int k = 0; k < from.size2(); ++k)
            {
               Node nd2;
               nd2.push_back(from(i, k).real());
               nd2.push_back(from(i, k).imag());
               nd1.push_back(nd2);
            }
            nd.push_back(nd1);
         }
         return nd;
      }

      static bool decode(const Node& nd, UblasMatrix<Complex> & to)
      {
         if(!nd.IsSequence())
         {
            return false;
         }
         else
         {
            int nrows = nd.size();
            if (nrows == 0)
            {
               std::cerr << "Matrix has zero rows in yaml." << std::endl;
               return false;
            }
            int ncols = nd[0].size();
            if (ncols == 0)
            {
               std::cerr << "Matrix has zero columns in yaml." << std::endl;
               return false;
            }
            for (int i = 1; i < nrows; ++i)
            {
               if (nd[i].size() != ncols)
               {
                  std::cerr << "Ill-formed matrix in yaml." << std::endl;
                  return false;
               }
            }
            to = UblasMatrix<Complex>(nrows, ncols);
            for (int i = 0; i < nrows; ++i)
            {
               for (int k = 0; k < nrows; ++k)
               {
                  to(i, k) = {nd[i][k][0].as<double>(), nd[i][k][1].as<double>()};
               }
            }
         }
         return true;
      }
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
