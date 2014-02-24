#ifndef INVERTER_BASE_DOT_H
#define INVERTER_BASE_DOT_H

#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/PowerFlow.h>
#include <SmartGridToolbox/ZipToGroundBase.h>

namespace SmartGridToolbox
{
   class DcPowerSourceBase;

   /// DC power to n-phase AC converter.
   class InverterBase : public ZipToGroundBase
   {
      public:
         virtual ublas::vector<Complex> Y() const override {return ublas::vector<Complex>(phases().size(), czero);}
         virtual ublas::vector<Complex> I() const override {return ublas::vector<Complex>(phases().size(), czero);}
         virtual ublas::vector<Complex> S() const override = 0;
      
      public:
         InverterBase(const std::string & name, const Phases & phases) : ZipToGroundBase(name, phases), PDc_(0.0)
         {
            // Empty.
         }

         void addDcPowerSource(DcPowerSourceBase & source);

         virtual double efficiency(double powerDc) const = 0;

         /// Real power output, per phase.
         virtual double PPerPhase() const;

      protected:
         virtual void updateState(Time t0, Time t1) override;

      
      private:
         std::vector<const DcPowerSourceBase*> sources_;    ///< My DC power sources.
         double PDc_;                                       ///< DC power = sum of all sources.
   };
}

#endif // INVERTER_BASE_DOT_H
