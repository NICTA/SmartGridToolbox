// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef MICROGRID_DEMO_CONTROLLER_DOT_H
#define MICROGRID_DEMO_CONTROLLER_DOT_H

#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/Parser.h>
#include <SmartGridToolbox/SimpleBattery.h>

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
         virtual void updateState(SmartGridToolbox::Time t) override;

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
