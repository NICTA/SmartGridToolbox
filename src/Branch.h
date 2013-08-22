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

   class BranchParser : public ComponentParser
   {
      public:
         static constexpr const char * getComponentName()
         {
            return "branch";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;

         virtual void postParse(const YAML::Node & nd, Model & mod) const override;
   };

   class Branch : public Component
   {
      /// @name My public member functions.
      /// @{
      public:
         Branch(const std::string & name) : Component(name) {}

         const Bus & getBus0() const {return *bus0_;}
         Bus & getBus0() {return *bus0_;}
         void setBus0(Bus & bus0) {bus0_ = &bus0;}

         const Bus & getBus1() const {return *bus1_;}
         Bus & getBus1() {return *bus1_;}
         void setBus1(Bus & bus1) {bus1_ = &bus1;}

         Phases getPhases0() const {return phases0_;}
         void setPhases0(Phases phases0) {phases0_ = phases0;}

         Phases getPhases1() const {return phases1_;}
         void setPhases1(Phases phases1) {phases1_ = phases1;}

         const UblasCMatrix<Complex> & getY() const {return Y_;}
         UblasCMatrix<Complex> & getY() {return Y_;}
         void setY(const UblasCMatrix<Complex> & Y) {Y_ = Y;}
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
