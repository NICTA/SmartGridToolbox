#ifndef INVERTER_DOT_H
#define INVERTER_DOT_H

#include <SgtSim/InverterBase.h>

namespace SmartGridToolbox
{
   /// @brief Inverter: DC power to n-phase AC converter.
   class Inverter : public InverterBase
   {
      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;
      
      /// @}
      
      /// @name Overridden member functions from InverterBase.
      /// @{
      
      public:
         virtual ublas::vector<Complex> S() const override;
         virtual double PPerPhase() const override;

      /// @}
      
      /// @name My public member functions.
      /// @{
      
      public:
         Inverter(const std::string& id, const Phases& phases);

         virtual double efficiency(double powerDc) const override
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
         void setInService(bool inService)
         {
            inService_ = inService;
         }

      /// @}
      
      public:
         // Operating parameters:
         double efficiency_;
         double maxSMagPerPhase_;
         double minPowerFactor_;

         // Settings:
         double requestedQPerPhase_;
         bool inService_;

         // State:
         ublas::vector<Complex> S_;
   };
}

#endif // INVERTER_DOT_H
