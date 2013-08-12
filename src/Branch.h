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
      /// @name Public overridden functions: from Component.
      /// @{
      public:
         virtual ptime getValidUntil() const override 
         {
            return pos_infin;
         }
      /// @}

      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState(ptime t) override {};
         virtual void updateState(ptime t0, ptime t1) override {};
      /// @}

      /// @name My public member functions.
      /// @{
      public:
         Branch(const std::string & name) : Component(name) {}

         const Bus & getBusi() const {return *busi_;}
         Bus & getBusi() {return *busi_;}
         void setBusi(Bus & busi) {busi_ = &busi;}

         const Bus & getBusk() const {return *busk_;}
         Bus & getBusk() {return *busk_;}
         void setBusk(Bus & busk) {busk_ = &busk;}

         const UblasCMatrix<Complex> & getY() const {return Y_;}
         UblasCMatrix<Complex> & getY() {return Y_;}
         void setY(const UblasCMatrix<Complex> & Y) {Y_ = Y;}
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         Bus * busi_;             ///< My i bus.
         Bus * busk_;             ///< My k bus.
         UblasCMatrix<Complex> Y_;  ///< Complex value of elements in bus admittance matrix in NR solver.
      /// @}
   };
}

#endif // BRANCH_DOT_H
