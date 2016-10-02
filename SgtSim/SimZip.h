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

namespace Sgt
{
    /// @brief Abstract base class for SimZip.
    /// 
    /// Depending on how the derived class works, zip() could either be provided by containment or inheritance.
    class SimZipAbc : virtual public SimComponent
    {
        public:

            /// @brief Return the zip that I wrap (const). 
            virtual const ZipAbc& zip() const = 0;
            /// @brief Return the zip that I wrap (non-const). 
            virtual ZipAbc& zip() = 0;
    };

    /// @brief Simulation zip, corresponding to a ZipAbc in a SimNetwork's network(). 
    /// 
    /// zip() is provided by containment which is enough for a normal network zip types. 
    class SimZip : public SimZipAbc
    {
        public:

            SimZip(const std::string& id, const ComponentPtr<ZipAbc>& zip) :
                Component(id),
                zip_(zip)
            {
                // Empty.
            }

            const ZipAbc& zip() const override
            {
                return *zip_;
            }
            
            ZipAbc& zip() override
            {
                return *zip_;
            }

        private:

            ComponentPtr<ZipAbc> zip_;
    };
    
    /// @brief Do anything needed to do to add simZip to the simNetwork.
    ///
    /// Important: simZip's zip must separately be added to simNetwork's network. This is to prevent any possible
    /// confusion about whether it is already added on not.
    void link(const ConstSimComponentPtr<SimZipAbc>& simZip, SimNetwork& simNetwork);
}

#endif // SIM_ZIP_DOT_H
