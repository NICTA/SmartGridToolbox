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

#ifndef REGULAR_UPDATE_COMPONENT_DOT_H
#define REGULAR_UPDATE_COMPONENT_DOT_H

#include <SgtSim/SimComponent.h>

namespace Sgt
{
    /// @brief Utility base class for a component that updates with a regular "tick" dt.
    class HeartbeatAdaptor : public SimComponentAdaptor
    {
        /// @name Lifecycle:
        /// @{

        public:

            HeartbeatAdaptor(const Time& dt) :
                dt_(dt)
            {
                // Empty.
            }

        /// @}

        /// @name Overridden member functions from SimComponent.
        /// @{

        public:

            virtual Time validUntil() const override
            {
                return nextBeat_;
            }

        protected:

            virtual void initializeState() override
            {
                nextBeat_ = posix_time::not_a_date_time;
                needsUpdate().trigger(); // Update on the first timestep.
            }

            virtual void updateState(Time t) override
            {
                if (nextBeat_ == posix_time::not_a_date_time)
                {
                    nextBeat_ = t; // OK because I'm guaranteed to update on first timestep.
                }

                if (t == nextBeat_)
                {
                    nextBeat_ += dt_;
                };
            }

        /// @}

        /// @name Heartbeat specific member functions.
        /// @{

        public:

            Time dt() const
            {
                return dt_;
            }

            void setDt(Time dt)
            {
                dt_ = dt;
            }

        /// @}

        private:
            Time dt_;

            Time nextBeat_{posix_time::not_a_date_time};
    };

    class Heartbeat : virtual public Component, public HeartbeatAdaptor
    {
        public:

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("heartbeat");
                return result;
            }

        /// @}

        /// @name Lifecycle:
        /// @{

            Heartbeat(const std::string& id, const Time& dt) :
                HeartbeatAdaptor(dt),
                Component(id)
            {
                // Empty.
            }

        /// @}

        /// @name Component virtual overridden functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual void print(std::ostream& os) const override; TODO

        /// @}
    };
}

#endif // REGULAR_UPDATE_COMPONENT_DOT_H
