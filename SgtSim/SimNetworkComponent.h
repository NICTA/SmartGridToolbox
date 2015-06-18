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

#ifndef SIM_NETWORK_COMPONENT_DOT_H
#define SIM_NETWORK_COMPONENT_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Bus.h>
#include <SgtCore/CommonBranch.h>
#include <SgtCore/DgyTransformer.h>
#include <SgtCore/Gen.h>
#include <SgtCore/OverheadLine.h>
#include <SgtCore/SinglePhaseTransformer.h>
#include <SgtCore/YyTransformer.h>
#include <SgtCore/Zip.h>

#include <memory>

namespace Sgt
{
    class SimBus : public SimComponent
    {
        public:

            SimBus(std::shared_ptr<Bus> bus) :
                SimComponent(bus->id()),
                bus_(bus)
            {
                // Empty.
            }

            std::shared_ptr<const Bus> bus() const
            {
                return bus_;
            }
            
            std::shared_ptr<Bus> bus()
            {
                return bus_;
            }

            void joinNetwork(SimNetwork& simNetwork)
            {
                simNetwork.dependsOn(this);
                simBus->bus()->setpointChanged().addAction([this]() {needsUpdate().trigger();},
                        std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
                simBus->bus()->isInServiceChanged().addAction([this]() {needsUpdate().trigger();},
                        std::string("Trigger ") + sComponentType() + " " + id() + " needs update");
            }

        protected:

            virtual void initializeState() override
            {
                bool sv = bus_->voltageUpdated().isEnabled();
                bus_->voltageUpdated().setIsEnabled(false);
                bus_->setV(bus_->VNom());
                bus_->voltageUpdated().setIsEnabled(sv);
            }

        private:

            std::shared_ptr<Bus> bus_;
    };

    class SimBranch : public SimComponent
    {
        public:

            SimBranch(std::shared_ptr<BranchAbc> branch) :
                SimComponent(branch->id()),
                branch_(branch)
            {
                // Empty.
            }

            std::shared_ptr<const BranchAbc> branch() const
            {
                return branch_;
            }
            
            std::shared_ptr<BranchAbc> branch()
            {
                return branch_;
            }

        private:

            std::shared_ptr<BranchAbc> branch_;
    };

    class SimGen : public SimComponent
    {
        public:

            SimGen(std::shared_ptr<GenAbc> gen) :
                SimComponent(gen->id()),
                gen_(gen)
            {
                // Empty.
            }

            std::shared_ptr<const GenAbc> gen() const
            {
                return gen_;
            }
            
            std::shared_ptr<GenAbc> gen()
            {
                return gen_;
            }

        private:

            std::shared_ptr<GenAbc> gen_;
    };

    class SimZip : public SimComponent
    {
        public:

            SimZip(std::shared_ptr<ZipAbc> zip) :
                SimComponent(zip->id()),
                zip_(zip)
            {
                // Empty.
            }

            std::shared_ptr<const ZipAbc> zip() const
            {
                return zip_;
            }
            
            std::shared_ptr<ZipAbc> zip()
            {
                return zip_;
            }

        private:

            std::shared_ptr<ZipAbc> zip_;
    };
}

#endif // SIM_NETWORK_COMPONENT_DOT_H
