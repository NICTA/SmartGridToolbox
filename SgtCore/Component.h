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

namespace Sgt
{
    /// @brief Base class for all Components. 
    ///
    /// A Component is essentially an object with a unique key.
    /// It is usually a good idea to use virtual inheritance to derive from component.
    /// @ingroup Foundation
    class Component : public std::enable_shared_from_this<Component>, public HasProperties
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

            Component(const Component& from) = delete;

            virtual ~Component() = default;

            /// @}
            
            /// @name Virtual fuctions (to be overridden):
            /// @{

            virtual const std::string& id() const
            {
                return id_;
            }

            SGT_PROP_GET(id, id, const std::string&);

            virtual const std::string& componentType() const
            {
                return sComponentType();
            }

            SGT_PROP_GET(componentType, componentType, const std::string&);

            virtual void print(std::ostream& os) const
            {
                os << toJson().dump(2);
            }

            virtual json toJson() const
            {
                return {{"component", {{"id", id_}, {"component_type", componentType()}}}};
            }

            /// @}
            
            /// @name User data.
            /// @{

            /// @brief For application specific data, not used internally by SGT.
            const json& userData() const
            {
                return userData_;
            }
            
            json& userData()
            {
                return userData_;
            }
            
            void setUserData(const json& userData)
            {
                userData_ = userData;
            }

            SGT_PROP_GET_SET(userData, userData, const json&, setUserData, const json&);

            /// @}
        
        private:

            std::string id_;
            json userData_;
    };
      
    template<typename T, typename U = T> std::shared_ptr<U> shared(T& x)
    {
        return std::dynamic_pointer_cast<U>(x.shared_from_this());
    }

    inline std::ostream& operator<<(std::ostream& os, const Component& comp)
    {
        comp.print(os);
        return os;
    }
}

#endif // COMPONENT_DOT_H
