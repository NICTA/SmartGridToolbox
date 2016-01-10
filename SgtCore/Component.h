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
    // KLUDGE: TODO: Deriving Component from std::enable_shared_from_this<Component> would be ideal, but this triggers
    // a clang bug (apparently recently fixed but not in latest version of clang) preventing one from thereafter
    // constructing a std::shared_ptr<const Component>. Thus we need to add some machinery as a workaround. While we
    // are about it, we may as well add the other desired machinery for e.g. dynamic_pointer_casting, here in the
    // base class.
    template<typename T> class Shared : public std::enable_shared_from_this<const T>
    {
        public:

            // Hides std::enable_shared_from_this::shared_from_this.
            std::shared_ptr<const T> shared_from_this() const
            {
                return std::enable_shared_from_this<const T>::shared_from_this();
            }
            
            // Non-const version.
            std::shared_ptr<T> shared_from_this()
            {
                return std::const_pointer_cast<T>(std::enable_shared_from_this<const T>::shared_from_this());
            }

            template<typename U> std::shared_ptr<U> shared() const
            {
                return std::dynamic_pointer_cast<U>(shared_from_this());
            }

            template<typename U> std::shared_ptr<U> shared()
            {
                return std::dynamic_pointer_cast<U>(shared_from_this());
            }
    };

    /// @brief Base class for all Components. 
    ///
    /// A Component is essentially an object with a unique key.
    /// It is usually a good idea to use virtual inheritance to derive from component.
    /// @ingroup Foundation
    class Component : public Shared<Component>, public HasProperties<Component>
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
           
            /// Default constructor required by g++4.8 but not clang++.
            Component() : id_("UNDEFINED")
            {
                // Empty.
            }

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

            SGT_PROP_GET(id, Component, const std::string&, id);

            virtual const std::string& componentType() const
            {
                return sComponentType();
            }

            SGT_PROP_GET(componentType, Component, const std::string&, componentType);

            virtual void print(std::ostream& os) const
            {
                os << toJson().dump(2);
            }

            virtual json toJson() const
            {
                return {{"component", {{"id", id_}}}};
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
