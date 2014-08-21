#ifndef SIMPLE_BUILDING_DOT_H
#define SIMPLE_BUILDING_DOT_H

#include <SgtSim/Component.h>
#include <SgtSim/TimeSeries.h>
#include <SgtSim/Weather.h>
#include <SgtSim/ZipBase.h>

#include <SgtCore/Common.h>

#include<string>

namespace SmartGridToolbox
{
   enum class HvacMode
   {
      COOLING,
      HEATING,
      OFF
   };

   class SimpleBuilding : public ZipBase
   {
      /// @name Overridden member functions from SimObject.
      /// @{
      
      public:
         virtual Time validUntil() const override {return time() + dt_;}

      protected:
         virtual void initializeState() override;
         virtual void updateState(Time t) override;
      
      /// @}
      
      /// @name Overridden member functions from ZipBase.
      /// @{
      
      public:
         virtual ublas::vector<Complex> Y() const override {return {1, czero};}
         virtual ublas::vector<Complex> I() const override {return {1, czero};}
         virtual ublas::vector<Complex> S() const override {return {1, Complex(-Ph_, 0.0)};}

      /// @}
      
      /// @name My public member functions.
      /// @{
      
      public:
         SimpleBuilding(const std::string& name) :
            ZipBase(name, Phase::BAL),
            dt_(posix_time::minutes(5)),
            kb_(5 * kW / K),
            Cb_(1.0e5 * kJ / K),
            TbInit_(20.0 * K),
            kh_(10.0 * kW / K),
            copCool_(3.0),
            copHeat_(4.0),
            PMax_(20.0 * kW),
            Ts_(20.0 * K),
            weather_(nullptr),
            dQg_(nullptr),
            Tb_(0.0),
            mode_(HvacMode::OFF),
            cop_(0.0),
            isMaxed_(false),
            Ph_(0.0),
            dQh_(0.0)
         {
         }

         // Parameters:
         Time dt() {return dt_;}
         void set_dt(Time val) {dt_ = val; needsUpdate().trigger();}

         double kb() {return kb_;}
         void set_kb(double val) {kb_ = val;}

         double Cb() {return Cb_;}
         void setCb(double val) {Cb_ = val;}

         double TbInit() {return TbInit_;}
         void setTbInit(double val) {TbInit_ = val; needsUpdate().trigger();}

         double kh() {return kh_;}
         void set_kh(double val) {kh_ = val;}

         double copCool() {return copCool_;}
         void setCopCool(double val) {copCool_ = val;}

         double copHeat() {return copHeat_;}
         void setCopHeat(double val) {copHeat_ = val; needsUpdate().trigger();}

         double PMax() {return PMax_;}
         void setPMax(double val) {PMax_ = val;}

         double Ts() {return Ts_;}
         void setTs(double val) {Ts_ = val;}

         void setWeather(const Weather& weather) {weather_ = &weather; needsUpdate().trigger();}

         void set_dQgSeries(const TimeSeries<Time, double>& dQg) {dQg_ = &dQg; needsUpdate().trigger();}

         double Tb() {return Tb_;}

         const double Te() {return weather_->temperatureSeries()->value(time());}

         const double dQg() {return dQg_->value(time());}

         HvacMode mode() {return mode_;}

         double cop() {return cop_;}

         double isMaxed() {return isMaxed_;}

         double Ph() {return Ph_;}

         double dQh() {return dQh_;}
      
      /// @}

      /// @name My private member functions.
      /// @{
      
      private:
         void setOperatingParams(Time t);

      /// @}
      
      private:
         // Parameters and controls.
         Time dt_;                              // Timestep.
         double kb_;                            // Thermal conductivity, W/K.
         double Cb_;                            // Heat capacity of building, J/K.
         double TbInit_;                        // Initial temp of building.
         double kh_;                            // HVAC PID parameter, W/K.
         double copCool_;                       // HVAC cooling coeff. of perf.
         double copHeat_;                       // HVAC heating coeff. of perf.
         double PMax_;                          // HVAC max power, W.
         double Ts_;                            // HVAC set_point, C.

         const Weather* weather_;               // For external temperature.
         const TimeSeries<Time, double>* dQg_;  // Extra heat -> building.

         // State.
         double Tb_;                            // Building temperature, C.
                                                // Operating parameters.
         HvacMode mode_;                        // Cooling or heating?
         double cop_;                           // Depends on mode.
         bool isMaxed_;                         // On maximum power?
         double Ph_;                            // Power drawn from grid by HVAC.
         double dQh_;                           // Thermal output, +ve = heating.
   };
}
#endif // SIMPLE_BUILDING_DOT_H
