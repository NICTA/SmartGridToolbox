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

#ifndef INVERTER_DOT_H
#define INVERTER_DOT_H

#include <SgtSim/DcPowerSource.h>
#include <SgtSim/SimNetwork.h>

#include <memory>
#include <numeric>

namespace Sgt
{
    /// @brief DC power to n-phase AC converter.
    class InverterAbc : virtual public SimComponent
    {
        public:

            /// @name Lifecycle
            /// @{

            virtual ~InverterAbc() = default;

            /// @}

            /// @name Inverter specific member functions.
            /// @{

            virtual void addDcPowerSource(const ConstSimComponentPtr<DcPowerSourceAbc>& source);

            virtual double efficiency(double powerDc) const = 0;

            /// @brief Total DC power from all sources.
            virtual double PDc() const
            {
                return std::accumulate(sources_.begin(), sources_.end(), 0.0,
                        [] (double tot, const ConstSimComponentPtr<DcPowerSourceAbc>& source)
                        {return tot + source->PDc();});
            }

            /// @brief Real power output.
            virtual double availableP() const;

            /// @}

        private:

            std::vector<ConstSimComponentPtr<DcPowerSourceAbc>> sources_;   ///< My DC power sources.
    };

    /// @brief An inverter with simple constant DC-AC and AC-DC efficiencies.
    class SimpleInverterAbc : public InverterAbc
    {
        public:

            /// @name Lifecycle
            /// @{

            SimpleInverterAbc(double effDcToAc = 1.0, double effAcToDc = 1.0) : 
                Component(""),
                effDcToAc_(effDcToAc),
                effAcToDc_(effAcToDc)
            {
                // Empty.
            }

            /// @}

            /// @name InverterAbc virtual overridden member functions.
            /// @{

            virtual double efficiency(double powerDc) const override
            {
                return powerDc >= 0 ? effDcToAc_ : effAcToDc_;
            }

            /// @}

            /// @name Efficiency.
            /// @{

            double efficiencyDcToAc() const
            {
                return effDcToAc_;
            }

            void setEfficiencyDcToAc(double effDcToAc)
            {
                effDcToAc_ = effDcToAc;
            }

            double efficiencyAcToDc() const
            {
                return effAcToDc_;
            }

            void setEfficiencyAcToDc(double effAcToDc)
            {
                effAcToDc_ = effAcToDc;
            }

            /// @}

        private:

            double effDcToAc_;
            double effAcToDc_;
    };

    class Zip;

    /// @brief DC power to n-phase AC converter.
    class Inverter : public SimpleInverterAbc, public SimZip
    {
        public:

            /// @name Static member functions:
            /// @{

            static const std::string& sComponentType()
            {
                static std::string result("simple_zip_inverter");
                return result;
            }

            /// @}

            /// @name Lifecycle.
            /// @{
            
            Inverter(const std::string& id, const ComponentPtr<Zip>& zip, SimNetwork& simNetwork,
                    double efficiency = 1.0) :
                Component(id),
                SimpleInverterAbc(efficiency),
                SimZip(zip, simNetwork)
            {
                // Empty.
            }

            virtual ~Inverter() = default;
            
            /// @}

            /// @name Component virtual overridden member functions.
            /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual json toJson() const override; TODO

            /// @}

            /// @name InverterAbc virtual overridden member functions.
            /// @{

            virtual void addDcPowerSource(const ConstSimComponentPtr<DcPowerSourceAbc>& source) override;

            /// @}
            
            /// @name Inverter specific member functions.
            /// @{

            double maxSMag() const
            {
                return maxSMag_;
            }

            void setMaxSMag(double maxSMag)
            {
                maxSMag_ = maxSMag;
            }

            double requestedQ() const
            {
                return requestedQ_;
            }

            void setRequestedQ(double requestedQ)
            {
                requestedQ_ = requestedQ;
            }

            /// @}

        protected:
            virtual void updateState(const Time& t) override;
            
        private:
            virtual arma::Mat<Complex> SConst() const;

        private:

            double maxSMag_{1e9};
            double requestedQ_{0.0};
    };
}

#endif // INVERTER_DOT_H
