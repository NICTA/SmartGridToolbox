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
    /// A `Component` is essentially an object with a unique key.
    /// It is usually a good idea to use virtual inheritance to derive from component.
    /// @ingroup Components
    class Component : public std::enable_shared_from_this<Component>, public HasProperties
    {
        public:

        /// @private
        SGT_PROPS_INIT(Component);

        /// @name Static member functions:
        /// @{

        /// @brief Return a component type string.
        static const std::string& sComponentType()
        {
            static std::string result("component");
            return result;
        }

        /// @}

        /// @name Lifecycle:
        /// @{

        /// Default constructor required by g++4.8 but not clang++.
        ///
        /// Should not generally be used.
        Component() : id_("UNDEFINED")
        {
            // Empty.
        }

        /// @brief Component constructor.
        Component(const std::string& id) : id_(id)
        {
            // Empty.
        }

        Component(const Component& from) = delete;

        virtual ~Component() override = default;

        /// @}

        /// @name Virtual fuctions (to be overridden):
        /// @{

        /// @brief Component ID.
        virtual const std::string& id() const
        {
            return id_;
        }

        /// @private
        SGT_PROP_GET(id, id, const std::string&);

        /// @brief Return the component type string.
        ///
        /// Remember to override for derived classes.
        virtual const std::string& componentType() const
        {
            return sComponentType();
        }

        /// @private
        SGT_PROP_GET(componentType, componentType, const std::string&);

        /// @brief Print component description to an `ostream`.
        virtual void print(std::ostream& os) const
        {
            os << toJson().dump(2);
        }

        /// @brief Produce a JSON representation of the component. 
        virtual json toJson() const
        {
            return {{"component", {{"id", id_}, {"component_type", componentType()}}}};
        }

        /// @}

        /// @name User data:
        /// @{

        /// @brief User data.
        ///
        /// For application specific data, not used internally by SGT.
        const json& userData() const
        {
            return userData_;
        }

        /// @brief User data.
        ///
        /// For application specific data, not used internally by SGT.
        json& userData()
        {
            return userData_;
        }

        /// @brief Set user data.
        ///
        /// For application specific data, not used internally by SGT.
        void setUserData(const json& userData)
        {
            userData_ = userData;
        }

        /// @private
        SGT_PROP_GET_SET(userData, userData, const json&, setUserData, const json&);

        /// @}

        private:

        std::string id_;
        json userData_;
    };

    /// @brief Get `std::shared_pointer` from a `Component`.
    template<typename T, typename U = T> std::shared_ptr<U> shared(T& x)
    {
        return std::dynamic_pointer_cast<U>(x.shared_from_this());
    }

    /// @brief Stream insertion operator for `Component`s.
    inline std::ostream& operator<<(std::ostream& os, const Component& comp)
    {
        comp.print(os);
        return os;
    }
}

#endif // COMPONENT_DOT_H
