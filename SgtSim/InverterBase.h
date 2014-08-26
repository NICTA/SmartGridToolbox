#ifndef INVERTER_BASE_DOT_H
#define INVERTER_BASE_DOT_H

#include <SgtSim/SimObject.h>

#include <SgtCore/ZipAbc.h>

namespace SmartGridToolbox
{
   class DcPowerSourceBase;

   /// @brief DC power to n-phase AC converter.
   /// @ingroup PowerFlowCore
   class InverterBase : public ZipAbc, public SimObject
   {
      /// @name Overridden member functions from SimObject.
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
         
         InverterBase(const std::string& name, const Phases& phases) : ZipAbc(name, phases), PDc_(0.0)
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


         void addDcPowerSource(DcPowerSourceBase& source);

         virtual double efficiency(double powerDc) const = 0;

         /// @brief Real power output, per phase.
         virtual double PPerPhase() const;
         
      /// @}

      private:
         std::vector<std::shared_ptr<DcPowerSourceBase>> sources_;   ///< My DC power sources.
         double PDc_;                                                ///< DC power = sum of all sources.
   };
}

#endif // INVERTER_BASE_DOT_H
