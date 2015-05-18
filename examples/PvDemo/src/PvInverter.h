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

#ifndef PV_INVERTER_DOT_H
#define PV_INVERTER_DOT_H

#include <SgtCore/Gen.h>
#include <SgtSim/Inverter.h>
#include <SgtCore/Parser.h>
#include <SgtSim/SimParser.h>

namespace Sgt
{
    class PvInverter : public SimpleInverterAbc, public GenericGen
    {
        public:

            /// @name Static member functions:
            /// @{

        public:

            static const std::string& sComponentType()
            {
                static std::string result("pv_inverter");
                return result;
            }

            /// @}

            PvInverter(const std::string& id, std::string busId) :
                GenericGen(id, Phase::BAL),
                busId_(busId)
            {
                setPMin(0.0);
            }

            /// @name ComponentInterface virtual overridden functions.
            /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            /// @}

            /// @name InverterAbc virtual overridden functions.
            /// @{

            virtual void addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source) override
            {
                InverterAbc::addDcPowerSource(source);
                source->dcPowerChanged().addAction([this]() {PChanged();}, "Update max power");
            }

            /// @}

            /// @name SimpleZipInverter specific member functions.
            /// @{

            double maxSMag() const
            {
                return maxSMag_;
            }

            void setMaxSMag(double maxSMag)
            {
                maxSMag_ = maxSMag;
            }

            /// @}

        public:

            std::string busId_;
            double maxSMag_{1e9};
            double maxQ_{1e9};
        
        private:

            void PChanged();
    };

    class PvInverterParserPlugin : public SimParserPlugin
    {
        public:
            virtual const char* key() override
            {
                return "pv_inverter";
            }

        public:
            virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // PV_INVERTER_DOT_H
