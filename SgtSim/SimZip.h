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

#ifndef SIM_ZIP_DOT_H
#define SIM_ZIP_DOT_H

#include <SgtSim/SimComponent.h>
#include <SgtSim/SimNetwork.h>

#include <SgtCore/Zip.h>

#include <memory>

namespace Sgt
{
    /// @brief Abstract base class for SimZip.
    /// 
    /// Depending on how the derived class works, zip() could either be provided by containment or inheritance.
    class SimZipAbc : virtual public SimComponent
    {
        public:

            /// @brief Return the zip that I wrap (const). 
            virtual std::shared_ptr<const ZipAbc> zip() const = 0;
            /// @brief Return the zip that I wrap (non-const). 
            virtual std::shared_ptr<ZipAbc> zip() = 0;

            /// @brief Do anything I need to do to add myself to the simNetwork.
            ///
            /// Important: my zip must separately be added to SimNetwork's Network. This is to prevent any possible
            /// confusion about whether it is already added on not.
            virtual void linkToSimNetwork(SimNetwork& simNetwork);
    };

    /// @brief Simulation zip, corresponding to a ZipAbc in a SimNetwork's network(). 
    /// 
    /// zip() is provided by containment which is enough for a normal network zip types. 
    class SimZip : public SimZipAbc
    {
        public:

            SimZip(std::shared_ptr<ZipAbc> zip) :
                Component(zip->id()),
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

#endif // SIM_ZIP_DOT_H
