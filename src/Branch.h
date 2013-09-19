#ifndef BRANCH_DOT_H
#define BRANCH_DOT_H

#include "Common.h"
#include "Component.h"
#include "Parser.h"
#include "PowerFlow.h"

#include <iostream>

namespace SmartGridToolbox
{
   class Bus;

   class BranchParser : public ParserPlugin
   {
      public:
         static constexpr const char * componentName()
         {
            return "branch";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };

   class Branch : public Component
   {
      /// @name My public member functions.
      /// @{
      public:
         Branch(const std::string & name) : Component(name) {}

         const Bus & bus0() const {return *bus0_;}
         void setBus0(Bus & bus0) {bus0_ = &bus0;}

         const Bus & bus1() const {return *bus1_;}
         void setBus1(Bus & bus1) {bus1_ = &bus1;}

         const Phases & phases0() const {return phases0_;}
         Phases & phases0() {return phases0_;}

         const Phases & phases1() const {return phases1_;}
         Phases & phases1() {return phases1_;}

         const UblasCMatrix<Complex> & Y() const {return Y_;}
         UblasCMatrix<Complex> & Y() {return Y_;}
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         Bus * bus0_;                  ///< My bus 0.
         Bus * bus1_;                  ///< My bus 1.
         Phases phases0_;              ///< Phases on bus 0.
         Phases phases1_;              ///< Phases on bus 1.
         UblasCMatrix<Complex> Y_;     ///< Complex value of elements in bus admittance matrix in NR solver.
      /// @}
   };
}

#endif // BRANCH_DOT_H
