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
    class Heartbeat : virtual public SimComponent
    {
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

        public:

            Heartbeat(const std::string& id, const Time& dt) :
                Component(id),
                dt_(dt)
            {
                // Empty.
            }

        /// @}

        /// @name Component virtual overridden member functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual json asJson() const override; TODO

        /// @}

        /// @name SimComponent virtual overridden member functions.
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
}

#endif // REGULAR_UPDATE_COMPONENT_DOT_H
