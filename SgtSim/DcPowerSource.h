#ifndef DC_POWER_SOURCE_DOT_H
#define DC_POWER_SOURCE_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Component.h>

namespace Sgt
{
    /// @brief DC power source.
    ///
    /// Abstract base class for any object that can provide a source of DC power, i.e. a single real power.
    /// @ingroup PowerFlowCore
    class DcPowerSourceAbc : public SimComponent
    {
        public:

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("dc_power_source");
                return result;
            }

        /// @}

        /// @name Lifecycle.
        /// @{

            DcPowerSourceAbc(const std::string& id) : SimComponent(id) {}

        /// @}

        /// @name ComponentInterface virtual overridden functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual void print(std::ostream& os) const override; TODO

        /// @}

        /// @name DC Power.
        /// @{

            virtual double PDc() const = 0;

        /// @}

        /// @name Events.
        /// @{

            /// @brief Event triggered when I go in or out of service.
            virtual Event& dcPowerChanged()
            {
                return dcPowerChanged_;
            }

        /// @}

        private:

            Event dcPowerChanged_{std::string(sComponentType()) + ": DC power changed"};
    };

    class GenericDcPowerSource : virtual public DcPowerSourceAbc
    {
        public:

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("generic_dc_power_source");
                return result;
            }

        /// @}

        /// @name Lifecycle
        /// @{

            GenericDcPowerSource(const std::string& id) : DcPowerSourceAbc(id), PDc_(0.0) {}

        /// @}

        /// @name ComponentInterface virtual overridden functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual void print(std::ostream& os) const override; TODO

        /// @}

        /// @name DC Power.
        /// @{

            virtual double PDc() const {return PDc_;}
            void setPDc(double PDc) {PDc_ = PDc; dcPowerChanged().trigger();}

        /// @}

        private:
            double PDc_;
    };
}

#endif // DC_POWER_SOURCE_DOT_H
