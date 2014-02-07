#ifndef MICROGRID_DEMO_CONTROLLER_DOT_H
#define MICROGRID_DEMO_CONTROLLER_DOT_H

#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/Parser.h>
#include <SmartGridToolbox/SimpleBattery.h>

extern "C" {
#include <gurobi_c.h>
}

namespace MicrogridDemo
{
   class MicrogridControllerParser : public SmartGridToolbox::ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "microgrid_controller";
         }

      public:
         virtual void parse(const YAML::Node & nd, SmartGridToolbox::Model & mod,
                            const SmartGridToolbox::ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, SmartGridToolbox::Model & mod,
                                const SmartGridToolbox::ParserState & state) const override;
   };

   class MicrogridController : public SmartGridToolbox::Component
   {
      public:
         MicrogridController(const std::string & name) : Component(name) {}
         
         void setMinP(double P) {minP_ = P;}
         void setMaxP(double P) {maxP_ = P;}

         void setBuildBus(SmartGridToolbox::Bus & bus);
         void setPvBus(SmartGridToolbox::Bus & bus);
         void setBatt(SmartGridToolbox::SimpleBattery & batt);

      protected:
         virtual void updateState(SmartGridToolbox::Time t0, SmartGridToolbox::Time t1) override;

      private:

      private:
         SmartGridToolbox::Bus * buildBus_;
         SmartGridToolbox::Bus * pvBus_;
         SmartGridToolbox::SimpleBattery * batt_;

         double minP_;
         double maxP_;
   };
};

#endif // PV_DEMO_CONTROLLER_DOT_H
