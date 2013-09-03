#ifndef INVERTER_BASE_DOT_H
#define INVERTER_BASE_DOT_H

#include "ZipToGroundBase.h"

namespace SmartGridToolbox
{
   class DCPowerSourceBase;

   /// InverterBase: DC power to n-phase AC converter.
   class InverterBase : public ZipToGroundBase
   {
      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState(Time t) override
         {
            updateState(t, t);
         }

         virtual void updateState(Time t0, Time t1) override;
      /// @}

      /// @name Public overridden member functions from ZipToGroundBase.
      /// @{
      public:
         virtual UblasVector<Complex> Y() const override {return UblasVector<Complex>(phases().size(), czero);}
         virtual UblasVector<Complex> I() const override {return UblasVector<Complex>(phases().size(), czero);}
         virtual UblasVector<Complex> S() const override; 
      /// @}
      
      /// @name My public member functions. 
      /// @{
      public:
         InverterBase(const std::string & name) : ZipToGroundBase(name) {}

         void addDCPowerSource(DCPowerSourceBase & source);

         virtual double efficiency(double powerDC) const = 0;

         /// Get the phase angle, as a function of DC power
         /** Note: this makes a limiting assumption that the phase angle of each phase is equal.
          *  If in the future this turns out to be too limiting, then we can use a vector of phase angles. */
         virtual double phaseAngleRadians(double powerDC) const = 0;
      /// @}
      
      /// @name My private member variables.
      /// @{
      private:
         std::vector<const DCPowerSourceBase *> sources_;   ///< My DC power sources.
         double PDC_;                                       ///< DC power = sum of all sources.
      /// @}
   };
}

#endif // INVERTER_BASE_DOT_H
