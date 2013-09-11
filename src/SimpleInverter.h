#ifndef SIMPLE_INVERTER_DOT_H
#define SIMPLE_INVERTER_DOT_H

#include "InverterBase.h"

namespace SmartGridToolbox
{
   class SimpleInverterParser : public ComponentParser
   {
      public:
         static constexpr const char * componentName()
         {
            return "inverter";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;

         virtual void postParse(const YAML::Node & nd, Model & mod) const override;
   };

   /// SimpleInverter: DC power to n-phase AC converter.
   class SimpleInverter : public InverterBase
   {
      /// @name Overridden from InverterBase
      /// @{
      UblasVector<Complex> S() const;
      /// @}
      
      /// @name My public member functions.
      /// @{
      public:
         SimpleInverter(const std::string & name) :
            InverterBase(name), 
            efficiency_(1.0), 
            maxPAppPerPhase_(5000.0), 
            minPowerFactor_(0.9), 
            requestedQPerPhase_(0.0), 
            inService_(true)
         {
            // Empty.
         }

         virtual double efficiency(double powerDC) const override
         {
            return efficiency_;
         }
         void setEfficiency(double efficiency)
         {
            efficiency_ = efficiency;
         }

         double maxPAppPerPhase() const
         {
            return maxPAppPerPhase_;
         }
         void setMaxPAppPerPhase(double maxPAppPerPhase)
         {
            maxPAppPerPhase_ = maxPAppPerPhase;
         }

         double minPhaseAngle() const
         {
            return minPhaseAngle_;
         }
         void setMinPhaseAngle(double minPhaseAngle)
         {
            minPhaseAngle_ = minPhaseAngle;
         }

         double requestedQPerPhase() const
         {
            return requestedQPerPhase_;
         }
         void setRequestedQPerPhase(double requestedQPerPhase)
         {
            requestedQPerPhase_ = requestedQPerPhase;
         }

         bool inService() const
         {
            return inService_;
         }
         bool setInService(bool inService)
         {
            inService_ = inService;
         }
      /// @}

      /// @name My private member functions.
      /// @{
      public:
      /// @}

      /// @name Private member variables.
      /// @{
      public:
         // Operating parameters:
         double efficiency_;
         double maxPAppPerPhase_;
         double minPhaseAngle_;
         double minPowerFactor_;

         // Settings:
         double requestedQPerPhase_;
         bool inService_;

         // State:
         UblasVector<Complex> S_;
      /// @}
   };
}

#endif // SIMPLE_INVERTER_DOT_H
