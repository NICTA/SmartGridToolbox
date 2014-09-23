#ifndef INVERTER_DOT_H
#define INVERTER_DOT_H

#include <SgtSim/DcPowerSource.h>
#include <SgtSim/SimNetworkComponent.h>

#include <numeric>

namespace SmartGridToolbox
{
   /// @brief DC power to n-phase AC converter.
   /// @ingroup PowerFlowCore
   class InverterAbc : public SimComponentAbc, public ZipAbc
   {
      public:
         
         InverterAbc(const std::string& id, const Phases& phases) : ZipAbc(id, phases)
         {
            // Empty.
         }

         static constexpr const char* sComponentType()
         {
            return "inverter";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
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

         void addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source);

         virtual double efficiency(double powerDc) const = 0;

         /// @brief Total DC power from all sources.
         double PDc() const
         {
            return std::accumulate(sources_.begin(), sources_.end(), 0.0,
                  [] (double tot, const std::shared_ptr<DcPowerSourceAbc>& source) 
                  {return tot + source->PDc();});
         }

         /// @brief Real power output, per phase.
         virtual double PPerPhase() const;

      /// @}
      
      private:

         std::vector<std::shared_ptr<DcPowerSourceAbc>> sources_;   ///< My DC power sources.
   };

   /// @brief Inverter: DC power to n-phase AC converter.
   class Inverter : public InverterAbc
   {
      public:

         Inverter(const std::string& id, const Phases& phases) : InverterAbc(id, phases) {}

         static constexpr const char* sComponentType()
         {
            return "sim_inverter";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }

         virtual ublas::vector<Complex> SConst() const override;

         virtual double efficiency(double powerDc) const override
         {
            return efficiency_;
         }

         void setEfficiency(double efficiency)
         {
            efficiency_ = efficiency;
         }
   
         virtual double PPerPhase() const;

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
         double efficiency_{1.0};
         double maxSMagPerPhase_{1e9};
         double minPowerFactor_{0.0};

         // Settings:
         double requestedQPerPhase_{0.0};
         bool inService_{true};

         // State:
         ublas::vector<Complex> S_{ublas::vector<Complex>(phases().size(), czero)};
   };
}

#endif // INVERTER_DOT_H
