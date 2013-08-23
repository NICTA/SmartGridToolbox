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
         virtual void initializeState(ptime t) override
         {
            updateState(t, t);
         }

         virtual void updateState(ptime t0, ptime t1) override;
      /// @}

      /// @name Public overridden member functions from ZipToGroundBase.
      /// @{
      public:
         virtual UblasVector<Complex> getY() const override {return UblasVector<Complex>(getPhases().size(), czero);}
         virtual UblasVector<Complex> getI() const override {return UblasVector<Complex>(getPhases().size(), czero);}
         virtual UblasVector<Complex> getS() const override; 
      /// @}
      
      /// @name My public member functions. 
      /// @{
      public:
         InverterBase(const std::string & name) : ZipToGroundBase(name) {}

         void addDCPowerSource(DCPowerSourceBase & source);

         virtual double getEfficiency(double powerDC) const = 0;

         /// Get the phase angle, as a function of DC power
         /** Note: this makes a limiting assumption that the phase angle of each phase is equal.
          *  If in the future this turns out to be too limiting, then we can use a vector of phase angles. */
         virtual double getPhaseAngleRadians(double powerDC) const = 0;
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
