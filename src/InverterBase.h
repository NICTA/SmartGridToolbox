#ifndef INVERTER_BASE_DOT_H
#define INVERTER_BASE_DOT_H

#include <SmartGridToolbox/ZipToGroundBase.h>

namespace SmartGridToolbox
{
   class DcPowerSourceBase;

   /// InverterBase: DC power to n-phase AC converter.
   class InverterBase : public ZipToGroundBase
   {
      /// @name Private overridden functions: from Component.
      /// @{
      protected:
         virtual void updateState(Time t0, Time t1) override;
      /// @}

      /// @name Public overridden member functions from ZipToGroundBase.
      /// @{
      public:
         virtual ublas::vector<Complex> Y() const override {return ublas::vector<Complex>(phases().size(), czero);}
         virtual ublas::vector<Complex> I() const override {return ublas::vector<Complex>(phases().size(), czero);}
         virtual ublas::vector<Complex> S() const override = 0;
      /// @}
      
      /// @name My public member functions. 
      /// @{
      public:
         InverterBase(const std::string & name, const Phases & phases) : ZipToGroundBase(name, phases)
         {
            // Empty.
         }

         void addDcPowerSource(DcPowerSourceBase & source);

         virtual double efficiency(double powerDc) const = 0;

         /// Real power output, per phase.
         virtual double PPerPhase() const;
      /// @}
      
      /// @name My private member variables.
      /// @{
      private:
         std::vector<const DcPowerSourceBase *> sources_;   ///< My DC power sources.
         double PDc_;                                       ///< DC power = sum of all sources.
      /// @}
   };
}

#endif // INVERTER_BASE_DOT_H
