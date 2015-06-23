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
    /// @brief Base class for all Components. 
    ///
    /// A Component is essentially an object with a unique key.
    /// It is usually a good idea to use virtual inheritance to derive from component.
    /// @ingroup Foundation
    class Component : public std::enable_shared_from_this<Component>, public HasProperties<Component>
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

            Component(const std::string& id) : id_(id)
            {
                // Empty.
            }

            Component(const Component& from) = default;

            virtual ~Component() = default;

            /// @}
            
            /// @name Shared pointer access:
            /// @{

            /// @brief Dynamic pointer cast to std::shared_ptr<T>
            ///
            /// For non-template shared_ptr<Component>, use shared_from_this()
            template<typename T> std::shared_ptr<T> shared()
            {
                return std::dynamic_pointer_cast<T>(shared_from_this());
            }
            
            /// @}

            /// @name Virtual fuctions (to be overridden):
            /// @{

            virtual const std::string& id() const
            {
                return id_;
            }

            SGT_PROP_GET(id, Component, const std::string&, id);

            virtual const std::string& componentType() const
            {
                return sComponentType();
            }

            SGT_PROP_GET(componentType, Component, const std::string&, componentType);

            virtual void print(std::ostream& os) const
            {
                os << componentType() << ": " << id() << ":" << std::endl;
            }

            /// @}
        
        private:

            std::string id_;
    };

    inline std::ostream& operator<<(std::ostream& os, const Component& comp)
    {
        comp.print(os);
        return os;
    }
}

#endif // COMPONENT_DOT_H
