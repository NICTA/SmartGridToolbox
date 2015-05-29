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

#ifndef COMPONENT_DOT_H
#define COMPONENT_DOT_H

#include <SgtCore/Properties.h>

#include <ostream>
#include <string>

using std::string;

namespace Sgt
{
    class ComponentInterface : virtual public HasPropertiesInterface
    {
        public:

        /// @name Lifecycle:
        /// @{

            virtual ~ComponentInterface() = default;

        /// @}

        /// @name Pure virtual fuctions (to be overridden):
        /// @{

            virtual const std::string& id() const = 0;

            virtual const std::string& componentType() const = 0;

            virtual void print(std::ostream& os) const = 0;

        /// @}
    };

    inline std::ostream& operator<<(std::ostream& os, const ComponentInterface& comp)
    {
        comp.print(os);
        return os;
    }

    class Component : virtual public ComponentInterface
    {
        public:

            SGT_PROPS_INIT(Component);

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("component");
                return result;
            }

        /// @}

        /// @name Lifecycle:
        /// @{

            Component(const std::string& id) :
                id_(id)
            {
                // Empty.
            }

        /// @}

        /// @name ComponentInterface virtual overridden functions.
        /// @{

            virtual const std::string& id() const
            {
                return id_;
            }
            
            SGT_PROP_GET(id, Component, const std::string&, id);

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            SGT_PROP_GET(componentType, Component, const std::string&, componentType);

            virtual void print(std::ostream& os) const override
            {
                os << componentType() << ": " << id() << ":" << std::endl;
            }

        /// @}

        private:
            std::string id_;
    };
}

#endif // COMPONENT_DOT_H
