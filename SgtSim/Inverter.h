#ifndef INVERTER_DOT_H
#define INVERTER_DOT_H

#include <SgtSim/SimZip.h>

namespace SmartGridToolbox
{
   class DcPowerSourceAbc;

   /// @brief DC power to n-phase AC converter.
   /// @ingroup PowerFlowCore
   class InverterAbc : public SimZipAbc
   {
      public:
         
         InverterAbc(const std::string& id, const Phases& phases) : SimZipAbc(id, phases)
         {
            // Empty.
         }
         
         virtual ublas::vector<Complex> YConst() const override
         {
            return ublas::vector<Complex>(phases().size(), czero);
         }
         virtual ublas::vector<Complex> IConst() const override
         {
            return ublas::vector<Complex>(phases().size(), czero);
         }
         virtual ublas::vector<Complex> SConst() const override = 0;

         virtual void addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source);

         virtual double efficiency(double powerDc) const = 0;

         /// @brief Real power output, per phase.
         virtual double PPerPhase() const
         {
            double PDcA = PDc();
            return PDcA * efficiency(PDcA) / phases().size();
         }

      /// @}
      
      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:
         virtual Time validUntil() const override;

      protected:
         virtual void initializeState() override;
         virtual void updateState(Time t) override;

         virtual double PDc() const;

      /// @}

      private:
         std::vector<std::shared_ptr<DcPowerSourceAbc>> sources_;   ///< My DC power sources.
   };

   /// @brief Inverter: DC power to n-phase AC converter.
   class Inverter : public InverterAbc
   {
      public:
         Inverter(const std::string& id, const Phases& phases);

         virtual ublas::vector<Complex> SConst() const override;

         virtual double PPerPhase() const override;

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
