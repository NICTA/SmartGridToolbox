#ifndef SIMPLE_BUILDING_DOT_H
#define SIMPLE_BUILDING_DOT_H

#include "SimpleBuilding.h"
#include "Common.h"
#include "Component.h"
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
         SimpleBuilding(const std::string & name, 
               time_duration dt = minutes(5),
               double kb = 0.0,
               double Cb = 0.0,
               double TbInit = 0.0,
               double kh = 0.0,
               double copCool = 0.0,
               double copHeat = 0.0,
               double Pmax = 0.0,
               double Ts = 0.0,
               const std::function<double (ptime)> & Te = 
                  [](ptime t){return 0.0;},
               const std::function<double (ptime)> & dQg = 
                  [](ptime t){return 0.0;}) :
            Component(name),
            dt_(dt),
            kb_(kb),
            Cb_(Cb),
            TbInit_(TbInit),
            kh_(kh),
            copCool_(copCool),
            copHeat_(copHeat),
            Pmax_(Pmax),
            Ts_(Ts),
            Te_(Te),
            dQg_(dQg),
            Tb_(0.0),
            mode_(HvacMode::OFF),
            cop_(0.0),
            isMaxed_(false),
            Ph_(0.0),
            dQh_(0.0)
         {
         }

         // Parameters:
         time_duration getdt() {return dt_;}
         void setdt(time_duration val) {dt_ = val;}

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

         const double getTe(ptime t) {return Te_(t);}
         void setTeFunc(const std::function<double (ptime)> & f) {Te_ = f;}

         const double getdQg(ptime t) {return dQg_(t);}
         void setdQgFunc(const std::function<double (ptime)> f) {dQg_ = f;}

         double getTb() {return Tb_;}

         HvacMode getMode() {return mode_;}

         double getCop() {return cop_;} 

         double getIsMaxed() {return isMaxed_;} 

         double getPh() {return Ph_;}

         double getdQh() {return dQh_;}

         virtual ptime getValidUntil() const
         {
            return getTime() + dt_;
         }

      private:
         virtual void initializeState(ptime t) override;
         virtual void updateState(ptime t0, ptime t1) override;

         void setOperatingParams(ptime t);

      private:
         // Parameters and controls.
         time_duration dt_;                  // Timestep.
         double kb_;                         // Thermal conductivity, W/K.
         double Cb_;                         // Heat capacity of building, J/K.
         double TbInit_;                     // Initial temp of building.
         double kh_;                         // HVAC PID parameter, W/K.
         double copCool_;                    // HVAC cooling coeff. of perf.
         double copHeat_;                    // HVAC heating coeff. of perf.
         double Pmax_;                       // HVAC max power, W.
         double Ts_;                         // HVAC setpoint, C.

         std::function<double (ptime)> Te_;  // External temperature.
         std::function<double (ptime)> dQg_; // Extra heat -> building.
                  
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
