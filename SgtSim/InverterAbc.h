#ifndef INVERTER_ABC_DOT_H
#define INVERTER_ABC_DOT_H

#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Zip.h>

namespace SmartGridToolbox
{
   class DcPowerSourceAbc;

   /// @brief DC power to n-phase AC converter.
   /// @ingroup PowerFlowCore
   class InverterAbc : public ZipAbc, virtual public SimZipInterface
   {
      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:
         virtual Time validUntil() const override;

      protected:
         virtual void initializeState() override;
         virtual void updateState(Time t) override;

      /// @}
      
      public:
      /// @name My public member functions.
      /// @{
         
         InverterAbc(const std::string& id, const Phases& phases) : ZipAbc(id, phases)
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

         void addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source);

         virtual double efficiency(double powerDc) const = 0;

         /// @brief Real power output, per phase.
         virtual double PPerPhase() const;

         virtual 
         
      /// @}

      private:
         std::vector<std::shared_ptr<DcPowerSourceAbc>> sources_;   ///< My DC power sources.
   };
}

#endif // INVERTER_ABC_DOT_H
