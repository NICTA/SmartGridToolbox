#ifndef SIMPLE_BUILDING_DOT_H
#define SIMPLE_BUILDING_DOT_H

#include "SimpleBuilding.h"
#include "Common.h"
#include "Component.h"
#include "TimeSeries.h"
#include<string>

namespace SmartGridToolbox
{
   enum class HvacMode
   {
      COOLING,
      HEATING,
      OFF
   };

   class SimpleBuilding : public Component
   {
      public:
         SimpleBuilding(const std::string & name):
            Component(name),
            kb_(0.0),
            Cb_(0.0),
            TbInit_(0.0),
            kh_(0.0),
            copCool_(1.0),
            copHeat_(2.0),
            Pmax_(0.0),
            Ts_(0.0),
            Te_(nullptr),
            dQg_(nullptr),
            Tb_(0.0),
            Ph_(0.0),
            dQh_(0.0)
         {
         }

         double getPower() 
         {
         }

         // Parameters:

         double getkb() {return kb_;}
         void setkb(double val) {kb_ = val;}

         double getCb() {return Cb_;}
         void setCb(double val) {Cb_ = val;}

         double getTbInit() {return TbInit_;}
         void setTbInit(double val) {TbInit_ = val;}

         double getkh() {return kh_;}
         void setkh(double val) {kh_ = val;}

         double getCopCool() {return copCool_;}
         void setCopCool(double val) {copCool_ = val;}

         double getCopHeat() {return copHeat_;}
         void setCopHeat(double val) {copHeat_ = val;}

         double getPmax() {return Pmax_;}
         void setPmax(double val) {Pmax_ = val;}

         double getTs() {return Ts_;}
         void setTs(double val) {Ts_ = val;}

         const TimeSeries<ptime, double> * getTe() {return Te_;}
         void setTe(const TimeSeries<ptime, double> & Te) {Te_ = &Te;}

         const TimeSeries<ptime, double> * getdQg() {return dQg_;}
         void setdQg(const TimeSeries<ptime, double> & dQg) {dQg_ = &dQg;}

         double getTb() {return Tb_;}

         HvacMode getMode() {return mode_;}

         double getCop() {return cop_;} 

         double getIsMaxed() {return isMaxed_;} 

         double getPh() {return Ph_;}

         double getdQh() {return dQh_;}

      private:
         virtual void initializeState(ptime t) override;
         virtual void updateState(ptime t0, ptime t1) override;

         void setOperatingParams(ptime t);

      private:
         // Parameters and controls.
         double kb_;                         // Thermal conductivity, W/K.
         double Cb_;                         // Heat capacity of building, J/K.
         double TbInit_;                     // Initial temp of building.
         double kh_;                         // HVAC PID parameter, W/K.
         double copCool_;                    // HVAC cooling coeff. of perf.
         double copHeat_;                    // HVAC heating coeff. of perf.
         double Pmax_;                       // HVAC max power, W.
         double Ts_;                         // HVAC setpoint, C.

         const TimeSeries<ptime, double> * Te_;    // External temperature.
         const TimeSeries<ptime, double> * dQg_;   // Extra heat -> building.
                  
         // State.
         double Tb_;                         // Building temperature, C.
         // Operating parameters.
         HvacMode mode_;                     // Cooling or heating?
         double cop_;                        // Depends on mode.
         bool isMaxed_;                      // On maximum power?
         double Ph_;                         // Power drawn from grid by HVAC.
         double dQh_;                        // Thermal output, +ve = heating.
   };
}
#endif // SIMPLE_BUILDING_DOT_H
