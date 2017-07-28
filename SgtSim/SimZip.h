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
    /// @brief Simulation zip, corresponding to a Zip in a SimNetwork's network(). 
    class SimZip : virtual public SimComponent
    {
        public:

            SimZip(const std::string& id, const ComponentPtr<Zip>& zip) :
                Component(id),
                zip_(zip)
            {
                // Empty.
            }

            const Zip& zip() const
            {
                return *zip_;
            }
            
            Zip& zip()
            {
                return *zip_;
            }

        private:

            ComponentPtr<Zip> zip_;
    };
    
    /// @brief Do anything needed to do to add simZip to the simNetwork.
    ///
    /// Important: simZip's zip must separately be added to simNetwork's network. This is to prevent any possible
    /// confusion about whether it is already added on not.
    void link(const ConstSimComponentPtr<SimZip>& simZip, SimNetwork& simNetwork);
}

#endif // SIM_ZIP_DOT_H
