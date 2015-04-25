#ifndef INVERTER_DOT_H
#define INVERTER_DOT_H

#include <SgtSim/DcPowerSource.h>
#include <SgtSim/SimNetworkComponent.h>

#include <numeric>

namespace Sgt
{
    /// @brief DC power to n-phase AC converter.
    /// @ingroup PowerFlowCore
    class InverterAbc : public SimComponentAdaptor, public ZipAbc
    {
        public:

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("inverter");
                return result;
            }

        /// @}

            InverterAbc(const std::string& id, const Phases& phases) : ZipAbc(id, phases)
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

            virtual arma::Col<Complex> YConst() const override
            {
                return arma::Col<Complex>(phases().size(), arma::fill::zeros);
            }
            virtual arma::Col<Complex> IConst() const override
            {
                return arma::Col<Complex>(phases().size(), arma::fill::zeros);
            }
            virtual arma::Col<Complex> SConst() const override = 0;

            void addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source);

            virtual double efficiency(double powerDc) const = 0;

            /// @brief Total DC power from all sources.
            double PDc() const
            {
                return std::accumulate(sources_.begin(), sources_.end(), 0.0,
                                       [] (double tot, const std::shared_ptr<DcPowerSourceAbc>& source)
                {return tot + source->PDc();});
            }

            /// @brief Real power output, per phase.
            virtual double P() const;

        /// @}

        private:

            std::vector<std::shared_ptr<DcPowerSourceAbc>> sources_;   ///< My DC power sources.
    };

    /// @brief Inverter: DC power to n-phase AC converter.
    class Inverter : public InverterAbc
    {
        public:

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("inverter");
                return result;
            }

        /// @}

            Inverter(const std::string& id, const Phases& phases) :
                InverterAbc(id, phases),
                S_(phases.size(), arma::fill::zeros)
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

            virtual arma::Col<Complex> SConst() const override;

            virtual double efficiency(double powerDc) const override
            {
                return efficiency_;
            }

            void setEfficiency(double efficiency)
            {
                efficiency_ = efficiency;
            }

            virtual double P() const;

            double maxSMag() const
            {
                return maxSMag_;
            }

            void setMaxSMag(double maxSMag)
            {
                maxSMag_ = maxSMag;
            }

            double minPowerFactor() const
            {
                return minPowerFactor_;
            }

            void setMinPowerFactor(double minPowerFactor)
            {
                minPowerFactor_ = minPowerFactor;
            }

            double requestedQ() const
            {
                return requestedQ_;
            }

            void setRequestedQ(double requestedQ)
            {
                requestedQ_ = requestedQ;
            }

            bool inService() const
            {
                return inService_;
            }

            void setInService(bool inService)
            {
                inService_ = inService;
            }

        /// @}

        public:
            // Operating parameters:
            double efficiency_{1.0};
            double maxSMag_{1e9};
            double minPowerFactor_{0.0};

            // Settings:
            double requestedQ_{0.0};
            bool inService_{true};

            // State:
            arma::Col<Complex> S_;
    };
}

#endif // INVERTER_DOT_H
