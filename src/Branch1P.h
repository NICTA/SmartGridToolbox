#ifndef BRANCH_1P_DOT_H
#define BRANCH_1P_DOT_H

#include "Common.h"
#include "Component.h"
#include "Parser.h"
#include "PowerFlow.h"

#include <iostream>

namespace SmartGridToolbox
{
   class Network1P;
   class Bus1P;

   class Branch1PParser : public ComponentParser
   {
      public:
         static constexpr const char * getComponentName()
         {
            return "branch_1_phase";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;

         virtual void postParse(const YAML::Node & nd, Model & mod) const override;
   };

   class Branch1P : public Component
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
         const Network1P & getNetwork() const {return *network_;}
         Network1P & getNetwork() {return *network_;}
         void setNetwork(Network1P & network) {network_ = &network;}

         const Bus1P & getBusi() const {return *busi_;}
         Bus1P & getBusi() {return *busi_;}
         void setBusi(Bus1P & busi) {busi_ = &busi;}

         const Bus1P & getBusk() const {return *busk_;}
         Bus1P & getBusk() {return *busk_;}
         void setBusk(Bus1P & busk) {busk_ = &busk;}

         const Array2D<Complex, 2, 2> & getY() const {return Y_;}
         Array2D<Complex, 2, 2> & getY() {return Y_;}
         void setY(const Array2D<Complex, 2, 2> & Y) {Y_ = Y;}
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         Network1P * network_;   ///< Network.
         Bus1P * busi_;          ///< My i bus.
         Bus1P * busk_;          ///< My k bus.
         Array2D<Complex, 2, 2> Y_;       ///< Complex value of elements in bus admittance matrix in NR solver.
      /// @}
   };
}

#endif // BRANCH_1P_DOT_H
