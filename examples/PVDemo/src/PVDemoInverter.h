#ifndef PV_DEMO_INVERTER_DOT_H
#define PV_DEMO_INVERTER_DOT_H

#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/SimpleInverter.h>

namespace PVDemo
{
   namespace ublas = SmartGridToolbox::ublas;
   using Complex = SmartGridToolbox::Complex;
   using SmartGridToolbox::czero;

   class PVDemoInverter : public SmartGridToolbox::SimpleInverter
   {
      public:
         PVDemoInverter(const SmartGridToolbox::SimpleInverter & from)
            : SmartGridToolbox::SimpleInverter(from),
              S_(czero)
         {
            // Empty.
         }

         virtual ublas::vector<Complex> S() const override {return {1, S_};}

         void setS(Complex S) {S_ = S;};

      private:
         virtual void updateState(SmartGridToolbox::Time t)
         {
            SmartGridToolbox::SimpleInverter::updateState(t);
            setS(SmartGridToolbox::SimpleInverter::S()(0));
         }

      private:
         Complex S_;
   };
}

#endif // PV_DEMO_INVERTER_DOT_H
