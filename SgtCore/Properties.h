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

#ifndef PROPERTIES_DOT_H
#define PROPERTIES_DOT_H

#include <SgtCore/json.h>
#include <SgtCore/YamlSupport.h>

#include<map>
#include<memory>
#include<stdexcept>
#include<string>
#include<sstream>
#include<type_traits>
#include<vector>

namespace Sgt
{
    using NoneType = std::nullptr_t;

    /// @ingroup Properties
    class NotGettableException : public std::logic_error
    {
        public:
        NotGettableException() : std::logic_error("Property is not gettable") {}
    };

    /// @ingroup Properties
    class NotSettableException : public std::logic_error
    {
        public:
        NotSettableException() : std::logic_error("Property is not settable") {}
    };

    /// @ingroup Properties
    class BadTargException : public std::logic_error
    {
        public:
        BadTargException() : std::logic_error("Target supplied to property has the wrong type") {}
    };

    class HasProperties;

    template<typename GetterRetType> class Getter;

    class GetterAbc
    {
        public:
        virtual ~GetterAbc() = default;

        template<typename TargType, typename GetterRetType> GetterRetType get(const TargType& targ)
        {
            auto derivedThis = dynamic_cast<const Getter<GetterRetType>*>(this);
            auto derivedTarg = dynamic_cast<const TargType*>(&targ);
            return derivedThis->get(derivedTarg);
        }
    };

    template<typename GetterRetType> class Getter : public GetterAbc
    {
        public:
        virtual ~Getter() = default;
        virtual GetterRetType get(const HasProperties& targ) const = 0;
    };

    template<typename TargType, typename GetterRetType> class ConcreteGetter : public Getter<GetterRetType>
    {
        public:
        template<typename T> ConcreteGetter(T getArg) : get_(getArg)
        {
            // Empty.
        }

        virtual GetterRetType get(const HasProperties& targ) const override
        {
            auto derived = dynamic_cast<const TargType*>(&targ);
            if (derived == nullptr)
            {
                throw BadTargException();
            }
            return get_(*derived);
        }

        private:
        std::function<GetterRetType(const TargType&)> get_;
    };

    template<typename SetterArgType> class Setter;

    class SetterAbc
    {
        public:
        virtual ~SetterAbc() = default;

        template<typename TargType, typename SetterArgType> void set(TargType& targ, SetterArgType val)
        {
            auto derivedThis = dynamic_cast<const Setter<SetterArgType>*>(this);
            auto derivedTarg = dynamic_cast<TargType*>(&targ);
            derivedThis->set(derivedTarg, val);
        }
    };

    template<typename SetterArgType> class Setter : public SetterAbc
    {
        public:
        virtual ~Setter() = default;
        virtual void set(HasProperties& targ, SetterArgType val) const = 0;
    };

    template<typename TargType, typename SetterArgType> class ConcreteSetter : public Setter<SetterArgType>
    {
        public:
        template<typename T> ConcreteSetter(T setArg) : set_(setArg)
        {
            // Empty.
        }

        virtual void set(HasProperties& targ, SetterArgType val) const override
        {
            auto derived = dynamic_cast<TargType*>(&targ);
            if (derived == nullptr)
            {
                throw BadTargException();
            }
            set_(*derived, val);
        }

        private:
        std::function<void(TargType&, SetterArgType)> set_;
    };

    /// @ingroup Properties
    class PropertyAbc
    {
        public:
        virtual ~PropertyAbc() = default;

        virtual bool isGettable() = 0;

        virtual const GetterAbc* getter() const
        {
            return getterAbc();
        }

        template<typename GetterRetType> GetterRetType getAs(const HasProperties& targ) const
        {
            auto derived = dynamic_cast<const Getter<GetterRetType>*>(getterAbc());
            return derived->get(targ);
        }

        virtual bool isSettable() = 0;

        virtual const SetterAbc* setter() const
        {
            return setterAbc();
        }

        template<typename SetterArgType> void setAs(HasProperties& targ, SetterArgType val) const
        {
            auto derived = dynamic_cast<const Setter<SetterArgType>*>(setterAbc());
            derived->set(targ, val);
        }

        virtual std::string string(const HasProperties& targ) = 0;

        virtual void setFromString(HasProperties& targ, const std::string& str) = 0;

        virtual Sgt::json json(const HasProperties& targ) = 0;


        private:

        virtual const GetterAbc* getterAbc() const = 0;
        virtual const SetterAbc* setterAbc() const = 0;
    };

    /// @brief Dynamically discoverable property.
    ///
    /// A property can have a getter and a setter. It can generically get its value and set via strings,
    /// or can get and set its the particular template type, if we know what that is.
    /// @ingroup Properties
    template<typename GetterRetType, typename SetterArgType> class Property : public PropertyAbc
    {
        public:

        Property(std::unique_ptr<Getter<GetterRetType>> getter, 
                std::unique_ptr<Setter<SetterArgType>> setter) :
            getter_(std::move(getter)),
            setter_(std::move(setter))
        {
            // Empty.
        }

        virtual bool isGettable() override
        {
            return getter_ != nullptr;
        }

        virtual const Getter<GetterRetType>* getter() const override
        {
            return getter_.get();
        }

        GetterRetType get(const HasProperties& targ) const
        {
            if (getter_ == nullptr)
            {
                throw NotGettableException();
            }
            return getter_->get(targ);
        }

        virtual bool isSettable() override
        {
            return setter_ != nullptr;
        }

        virtual const Setter<SetterArgType>* setter() const override
        {
            return setter_.get();
        }

        void set(HasProperties& targ, SetterArgType val) const
        {
            if (setter_ == nullptr)
            {
                throw NotSettableException();
            }
            setter_->set(targ, val);
        }

        virtual std::string string(const HasProperties& targ) override
        {
            return toYamlString(get(targ));
        }

        virtual void setFromString(HasProperties& targ, const std::string& str) override
        {
            set(targ, fromYamlString<std::decay_t<SetterArgType>>(str));
        }

        virtual Sgt::json json(const HasProperties& targ) override
        {
            return get(targ);
        }

        private:
        virtual const GetterAbc* getterAbc() const override
        {
            return getter_.get();
        }

        virtual const SetterAbc* setterAbc() const override
        {
            return setter_.get();
        }

        private:
        std::unique_ptr<Getter<GetterRetType>> getter_;
        std::unique_ptr<Setter<SetterArgType>> setter_;
    };

    /// @brief A collection of properties.
    /// @ingroup Properties
    class Properties
    {
        public:
        // Add a property with a getter.
        template<typename TargType, typename GetterRetType, typename GA>
            std::shared_ptr<Property<GetterRetType, NoneType>> addGetProperty(GA ga, const std::string& name)
            {
                auto result = std::make_shared<Property<GetterRetType, NoneType>>(
                        std::make_unique<ConcreteGetter<TargType, GetterRetType>>(ga), 
                        nullptr);
                map_[name] = result;
                return result;
            }

        // Add a property with a setter.
        template<typename TargType, typename SetterArgType, typename SA>
            std::shared_ptr<Property<NoneType, SetterArgType>> addSetProperty(SA sa, const std::string& name)
            {
                auto result = std::make_shared<Property<NoneType, SetterArgType>>(
                        nullptr,
                        std::make_unique<ConcreteSetter<TargType, SetterArgType>>(sa));
                map_[name] = result;
                return result;
            }

        // Add a property with a getter and a setter.
        template<typename TargType, typename GetterRetType, typename SetterArgType, typename GA, typename SA>
            std::shared_ptr<Property<GetterRetType, SetterArgType>> addGetSetProperty(
                    GA ga, SA sa, const std::string& name)
            {
                auto result = std::make_shared<Property<GetterRetType, SetterArgType>>(
                        std::make_unique<ConcreteGetter<TargType, GetterRetType>>(ga), 
                        std::make_unique<ConcreteSetter<TargType, SetterArgType>>(sa));
                map_[name] = result;
                return result;
            }

        const PropertyAbc& operator[](const std::string& s) const {return *map_.at(s);}
        PropertyAbc& operator[](const std::string& s) {return *map_.at(s);}

        auto begin() {return map_.begin();}
        auto end() {return map_.end();}
        auto cbegin() {return map_.begin();}
        auto cend() {return map_.end();}
        auto rbegin() {return map_.begin();}
        auto rend() {return map_.end();}

        template<typename ...Args> auto insert(Args... args) {return map_.insert(std::forward<Args>(args)...);}

        void remove(const std::string& key) {map_.erase(key);}

        private:
        std::map<std::string, std::shared_ptr<PropertyAbc>> map_;
    };

    /// @brief Abstract base class for an object with properties.
    /// @ingroup Properties
    class HasProperties
    {
        public:
        virtual ~HasProperties() = default;
        virtual Properties& properties() const = 0;
    };
}

/// @ingroup Properties
#define SGT_PROPS_INIT(Type) \
using TargType = Type; \
static Sgt::Properties& sProperties() \
{ \
    static Sgt::Properties props; \
    return props; \
} \
virtual Sgt::Properties& properties() const override {return sProperties();}

/// Copy all properties from the base class into this class. This allows an override mechanism, just like method
/// overrides. 
/// @ingroup Properties
#define SGT_PROPS_INHERIT(Base) \
struct Inherit ## Base \
{ \
    Inherit ## Base () {sProperties().insert(Base::sProperties().begin(), Base::sProperties().end());} \
}; \
struct DoInherit ## Base \
{ \
    DoInherit ## Base() {static Inherit ## Base inherit ## Base;} \
} doinherit ## Base \

/// Use a member function as a property getter.
/// @ingroup Properties
#define SGT_PROP_GET(name, getter, GetterRetType) \
struct InitProp_ ## name \
{ \
    InitProp_ ## name() \
    { \
        sProperties().addGetProperty<TargType, GetterRetType>( \
                [](const TargType& targ)->GetterRetType {return targ.getter();}, \
                #name); \
    } \
}; \
struct Prop_ ## name \
{ \
    Prop_ ## name() \
    { \
        static InitProp_ ## name _; \
    } \
} prop_ ## name;

/// Use a member function as a property setter.
/// @ingroup Properties
#define SGT_PROP_SET(name, setter, SetterArgType) \
struct InitProp_ ## name \
{ \
    InitProp_ ## name() \
    { \
        sProperties().addSetProperty<TargType, SetterArgType>( \
                [](TargType& targ, SetterArgType val) {targ.setter(val);}, \
                #name); \
    } \
}; \
struct Prop_ ## name { \
    Prop_ ## name() \
    { \
        static InitProp_ ## name _; \
    } \
} prop_ ## name

/// Use member functions as a property getters and setters.
/// @ingroup Properties
#define SGT_PROP_GET_SET(name, getter, GetterRetType, setter, SetterArgType) \
struct InitProp_ ## name \
{ \
    InitProp_ ## name() \
    { \
        sProperties().addGetSetProperty<TargType, GetterRetType, SetterArgType>( \
                [](const TargType& targ)->GetterRetType {return targ.getter();}, \
                [](TargType& targ, SetterArgType val) {targ.setter(val);}, \
                #name); \
    } \
}; \
struct Prop_ ## name \
{ \
    Prop_ ## name() \
    { \
        static InitProp_ ## name _; \
    } \
} prop_ ## name

#endif // PROPERTIES_DOT_H
