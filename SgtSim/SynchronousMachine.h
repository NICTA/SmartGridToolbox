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

#ifndef SYNCHRONOUS_MACHINE_DOT_H
#define SYNCHRONOUS_MACHINE_DOT_H

#include <iostream>

namespace Sgt
{
    class SynchronousMachine : public Component
    {
        /// @name Overridden member functions from SimComponent.
        /// @{

        public:
            // virtual Time validUntil() const override;

        protected:
            virtual void initializeState() override;
            virtual void updateState(Time t) override;

        /// @}

        /// @name My member functions.
        /// @{

        public:
            SynchronousMachine(const std::string& name);

            void addToNetwork(SimNetwork& nw);

        /// @}

        private:
            SimBus bus_;
            SimBranch branch_;
    }
}

#endif // SYNCHRONOUS_MACHINE_DOT_H
