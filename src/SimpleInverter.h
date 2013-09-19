#ifndef SIMPLE_INVERTER_DOT_H
#define SIMPLE_INVERTER_DOT_H

#include "InverterBase.h"

namespace SmartGridToolbox
{
   class SimpleInverterParser : public ParserPlugin
   {
      public:
         static constexpr const char * componentName()
         {
            return "inverter";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };

   /// SimpleInverter: DC power to n-phase AC converter.
   class SimpleInverter : public InverterBase
   {
      /// @name Overridden from InverterBase
      /// @{
      UblasVector<Complex> S() const override;
      virtual double PPerPhase() const override;
      /// @}
      
      /// @name My public member functions.
      /// @{
      public:
         SimpleInverter(const std::string & name) :
            InverterBase(name), 
            efficiency_(1.0), 
            maxSMagPerPhase_(5000.0), 
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

         double maxSMagPerPhase() const
         {
            return maxSMagPerPhase_;
         }
         void setMaxSMagPerPhase(double maxSMagPerPhase)
         {
            maxSMagPerPhase_ = maxSMagPerPhase;
         }

         double minPowerFactor() const
         {
            return minPowerFactor_;
         }
         void setMinPowerFactor(double minPowerFactor)
         {
            minPowerFactor_ = minPowerFactor;
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
         double maxSMagPerPhase_;
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
