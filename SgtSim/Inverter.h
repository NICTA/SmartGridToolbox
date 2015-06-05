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
#include <SgtSim/SimNetworkComponent.h>

#include <numeric>

namespace Sgt
{
    /// @brief DC power to n-phase AC converter.
    class InverterAbc : public SimComponentAdaptor
    {
        public:

        /// @name Inverter specific member functions.
        /// @{

            virtual void addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source);

            virtual double efficiency(double powerDc) const = 0;

            /// @brief Total DC power from all sources.
            double PDc() const
            {
                return std::accumulate(sources_.begin(), sources_.end(), 0.0,
                        [] (double tot, const std::shared_ptr<DcPowerSourceAbc>& source)
                        {return tot + source->PDc();});
            }

            /// @brief Real power output.
            virtual double availableP() const;

        /// @}

        private:

            std::vector<std::shared_ptr<DcPowerSourceAbc>> sources_;   ///< My DC power sources.
    };

    class SimpleInverterAbc : public InverterAbc
    {
        public:

        /// @name Lifecycle
        /// @{
            
            SimpleInverterAbc(double efficiency = 1.0) : efficiency_(efficiency)
            {
                // Empty.
            }

        /// @}
        
        /// @name InverterAbc virtual overridden functions.
        /// @{

            virtual double efficiency(double powerDc) const override
            {
                return efficiency_;
            }

        /// @{
        
        /// @name Efficiency.
        /// @{
     
            double efficiency() const
            {
                return efficiency_;
            }
            
            void setEfficiency(double efficiency)
            {
                efficiency_ = efficiency;
            }

        /// @{
        
        private:

            double efficiency_;
    };

    /// @brief DC power to n-phase AC converter.
    class SimpleZipInverter : public SimpleInverterAbc, public ZipAbc
    {
        public:

        /// @name Static member functions:
        /// @{

        public:

            static const std::string& sComponentType()
            {
                static std::string result("simple_zip_inverter");
                return result;
            }

        /// @}

            SimpleZipInverter(const std::string& id, const Phases& phases, double efficiency = 1.0) :
                SimpleInverterAbc(efficiency),
                ZipAbc(id, phases)
            {
                // Empty.
            }
        
        /// @name ComponentInterface virtual overridden functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual void print(std::ostream& os) const override; TODO

        /// @}
 
        /// @name InverterAbc virtual overridden functions.
        /// @{

            virtual void addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source) override;

        /// @}
       
        /// @name ZipAbc virtual overridden functions.
        /// @{

            virtual arma::Col<Complex> YConst() const override
            {
                return arma::Col<Complex>(phases().size(), arma::fill::zeros);
            }
            virtual arma::Col<Complex> IConst() const override
            {
                return arma::Col<Complex>(phases().size(), arma::fill::zeros);
            }
            virtual arma::Col<Complex> SConst() const override;

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

            double requestedQ() const
            {
                return requestedQ_;
            }

            void setRequestedQ(double requestedQ)
            {
                requestedQ_ = requestedQ;
            }

        /// @}
        
        private:
            
            double maxSMag_{1e9};
            double requestedQ_{0.0};
    };
}

#endif // INVERTER_DOT_H
