#ifndef SIMPLE_BUILDING_DOT_H
#define SIMPLE_BUILDING_DOT_H

#include "SimpleBuilding.h"
#include "Common.h"
#include "Component.h"
#include "Load.h"
#include<string>

namespace SmartGridToolbox
{
   enum class HvacMode
   {
      COOLING,
      HEATING,
      OFF
   };

   class SimpleBuilding : public Component, public Load
   {
      // Overridden functions: from Component.
      public:
         virtual ptime getValidUntil() const override {return getTime() + dt_;}

      private:
         virtual void initializeState(ptime t) override;
         virtual void updateState(ptime t0, ptime t1) override;

      // Overridden functions: from Load.
      public:
         virtual Complex getPower() const override {return Complex(Ph_, 0.0);}

      // Public member functions.
      public:
         SimpleBuilding(const std::string & name) :
            Component(name),
            dt_(minutes(5)),
            kb_(5 * kW / K),
            Cb_(1.0e5 * kJ / K),
            TbInit_(20.0 * K),
            kh_(10.0 * kW / K),
            copCool_(3.0),
            copHeat_(4.0),
            Pmax_(20.0 * kW),
            Ts_(20.0 * K),
            Te_([](ptime t){return 25.0;}),
            dQg_([](ptime t){return 10.0;}),
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
      
      // Private member functions:
      private:
         void setOperatingParams(ptime t);

      // Private member variables:
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
