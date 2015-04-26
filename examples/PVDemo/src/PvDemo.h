#ifndef PV_DEMO_DOT_H
#define PV_DEMO_DOT_H

#include <SgtCore/Gen.h>
#include <SgtCore/Parser.h>
#include <SgtCore/PowerFlowPtPpSolver.h>
#include <SgtSim/Inverter.h>
#include <SgtSim/SimParser.h>

class Net;
class PowerModel;

namespace Sgt
{
    class Network;
    class Simulation;

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
                // Empty.
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
                source->dcPowerChanged().addAction([this]() {setPMax(availableP());}, "Update max power");
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

            double maxSMag_{1e9};
            double requestedQ_{0.0};
            std::string busId_;
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

    class PvDemoSolver : public PowerFlowPtPpSolver
    {
        protected:
            virtual std::unique_ptr<PowerModel> makeModel(const Network& sgtNetw, Net& ptNetw) override;
    };
}

#endif // PV_DEMO_DOT_H
