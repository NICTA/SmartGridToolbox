#ifndef BRANCH_1P_COMPONENT_DOT_H
#define BRANCH_1P_COMPONENT_DOT_H

#include "Common.h"
#include "Component.h"
#include "Parser.h"
#include "PowerFlow.h"

#include <iostream>

namespace SmartGridToolbox
{
   class Network1PComponent;
   class Bus1PComponent;

   class Branch1PComponentParser : public ComponentParser
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

   class Branch1PComponent : public Component
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
         const Network1PComponent & getNetwork() const {return *network_;}
         Network1PComponent & getNetwork() {return *network_;}
         void setNetwork(Network1PComponent & network) {network_ = &network;}

         const Bus1PComponent & getBusi() const {return *busi_;}
         Bus1PComponent & getBusi() {return *busi_;}
         void setBusi(Bus1PComponent & busi) {busi_ = &busi;}

         const Bus1PComponent & getBusk() const {return *busk_;}
         Bus1PComponent & getBusk() {return *busk_;}
         void setBusk(Bus1PComponent & busk) {busk_ = &busk;}

         const Array2D<Complex, 2, 2> & getY() const {return Y_;}
         Array2D<Complex, 2, 2> & getY() {return Y_;}
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         Network1PComponent * network_;   ///< Network.
         Bus1PComponent * busi_;          ///< My i bus.
         Bus1PComponent * busk_;          ///< My k bus.
         Array2D<Complex, 2, 2> Y_;       ///< Complex value of elements in bus admittance matrix in NR solver.
      /// @}
   };
}

#endif // BRANCH_1P_COMPONENT_DOT_H
