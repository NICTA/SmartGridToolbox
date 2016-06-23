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

    class NotGettableException : public std::logic_error
    {
        public:
            NotGettableException() : std::logic_error("Property is not gettable") {}
    };

    class NotSettableException : public std::logic_error
    {
        public:
            NotSettableException() : std::logic_error("Property is not settable") {}
    };

    class BadTargException : public std::logic_error
    {
        public:
            BadTargException() : std::logic_error("Target supplied to property has the wrong type") {}
    };

    class PropertyAbc;

    using PropertyMap = std::map<std::string, std::shared_ptr<PropertyAbc>>;

    /// @brief Abstract base class for an object with properties.
    /// @ingroup Foundation
    class HasProperties
    {
        public:
            virtual ~HasProperties() = default;
            virtual PropertyMap& properties() const = 0;
    };

    template<typename RetType> class GetterAbc2;

    class GetterAbc1
    {
        public:
            virtual ~GetterAbc1() = default;

            template<typename TargType, typename RetType> RetType get(const TargType& targ)
            {
                auto derivedThis = dynamic_cast<const GetterAbc2<RetType>*>(this);
                auto derivedTarg = dynamic_cast<const TargType*>(&targ);
                return derivedThis->get(derivedTarg);
            }
    };

    template<typename RetType> class GetterAbc2 : public GetterAbc1
    {
        public:
            virtual ~GetterAbc2() = default;
            virtual RetType get(const HasProperties& targ) const = 0;
    };

    template<typename TargType, typename RetType> class Getter : public GetterAbc2<RetType>
    {
        public:
            template<typename T> Getter(T getArg) : get_(getArg)
            {
                // Empty.
            }

            virtual RetType get(const HasProperties& targ) const override
            {
                auto derived = dynamic_cast<const TargType*>(&targ);
                if (derived == nullptr)
                {
                    throw BadTargException();
                }
                return get_(*derived);
            }

        private:
            std::function<RetType(const TargType&)> get_;
    };

    template<typename ArgType> class SetterAbc2;

    class SetterAbc1
    {
        public:
            virtual ~SetterAbc1() = default;

            template<typename TargType, typename ArgType> void set(TargType& targ, ArgType val)
            {
                auto derivedThis = dynamic_cast<const SetterAbc2<ArgType>*>(this);
                auto derivedTarg = dynamic_cast<TargType*>(&targ);
                derivedThis->set(derivedTarg, val);
            }
    };

    template<typename ArgType> class SetterAbc2 : public SetterAbc1
    {
        public:
            virtual ~SetterAbc2() = default;
            virtual void set(HasProperties& targ, ArgType val) const = 0;
    };

    template<typename TargType, typename ArgType> class Setter : public SetterAbc2<ArgType>
    {
        public:
            template<typename T> Setter(T setArg) : set_(setArg)
            {
                // Empty.
            }

            virtual void set(HasProperties& targ, ArgType val) const override
            {
                auto derived = dynamic_cast<TargType*>(&targ);
                if (derived == nullptr)
                {
                    throw BadTargException();
                }
                set_(*derived, val);
            }

        private:
            std::function<void(TargType&, ArgType)> set_;
    };

    /// @ingroup Foundation
    class PropertyAbc
    {
        public:
            virtual ~PropertyAbc() = default;

            virtual bool isGettable() = 0;

            template<typename RetType> RetType getAs(const HasProperties& targ) const
            {
                auto derived = dynamic_cast<const GetterAbc2<RetType>*>(getterAbc1());
                return derived->get(targ);
            }

            virtual bool isSettable() = 0;

            template<typename ArgType> void setAs(HasProperties& targ, ArgType val) const
            {
                auto derived = dynamic_cast<const SetterAbc2<ArgType>*>(setterAbc1());
                derived->set(targ, val);
            }

            virtual std::string string(const HasProperties& targ) = 0;

            virtual json toJson(const HasProperties& targ) = 0;

            virtual void setFromString(HasProperties& targ, const std::string& str) = 0;

        private:

            virtual const GetterAbc1* getterAbc1() const = 0;
            virtual const SetterAbc1* setterAbc1() const = 0;
    };

    /// @brief Dynamically discoverable property.
    ///
    /// A property can have a getter and a setter. It can generically get its value and set via strings,
    /// or can get and set its the particular template type, if we know what that is.
    /// @ingroup Foundation
    template<typename GetterRetType, typename SetterArgType> class Property : public PropertyAbc
    {
        public:

            Property(std::unique_ptr<GetterAbc2<GetterRetType>> getter, 
                    std::unique_ptr<SetterAbc2<SetterArgType>> setter) :
                getter_(std::move(getter)),
                setter_(std::move(setter))
            {
                // Empty.
            }

            virtual bool isGettable() override
            {
                return getter_ != nullptr;
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

            void set(HasProperties& targ, SetterArgType val) const
            {
                if (setter_ == nullptr)
                {
                    throw NotSettableException();
                }
                setter_->set(targ, val);
            }

            virtual void setFromString(HasProperties& targ, const std::string& str) override
            {
                set(targ, fromYamlString<std::decay_t<SetterArgType>>(str));
            }

            virtual std::string string(const HasProperties& targ) override
            {
                return toYamlString(get(targ));
            }

            virtual json toJson(const HasProperties& targ) override
            {
                return get(targ);
            }

        private:
            virtual const GetterAbc1* getterAbc1() const override
            {
                return getter_.get();
            }

            virtual const SetterAbc1* setterAbc1() const override
            {
                return setter_.get();
            }

        private:
            std::unique_ptr<GetterAbc2<GetterRetType>> getter_;
            std::unique_ptr<SetterAbc2<SetterArgType>> setter_;
    };
}

#define SGT_PROPS_INIT(Type) \
using TargType = Type; \
static Sgt::PropertyMap& sPropertyMap() \
{ \
    static Sgt::PropertyMap map; \
    return map; \
} \
virtual std::map<std::string, std::shared_ptr<Sgt::PropertyAbc>>& properties() const override \
{ \
    return sPropertyMap(); \
}

/// Copy all properties from the base class into this class. This allows an override mechanism, just like method
/// overrides. 
#define SGT_PROPS_INHERIT(Base) \
struct Inherit ## Base \
{ \
    Inherit ## Base () \
    { \
        auto& targMap = sPropertyMap(); \
        auto& baseMap = Base::sPropertyMap(); \
        for (auto& elem : baseMap) \
        { \
            targMap[elem.first] = elem.second; \
        } \
    } \
}; \
struct DoInherit ## Base \
{ \
    DoInherit ## Base() \
    { \
        static Inherit ## Base inherit ## Base; \
    } \
} doinherit ## Base \

/// Use a member function as a property getter.
#define SGT_PROP_GET(name, getter, RetType) \
struct InitProp_ ## name \
{ \
    InitProp_ ## name() \
    { \
        sPropertyMap()[#name] = std::make_shared<Sgt::Property<RetType, Sgt::NoneType>>( \
            std::make_unique<Sgt::Getter<TargType, RetType>>( \
                [](const TargType& targ)->RetType \
                { \
                    return targ.getter(); \
                }), nullptr); \
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
#define SGT_PROP_SET(name, setter, ArgType) \
struct InitProp_ ## name \
{ \
    InitProp_ ## name() \
    { \
        sPropertyMap()[#name] = std::make_shared<Sgt::Property<Sgt::NoneType, ArgType>>(nullptr, \
                std::make_unique<Sgt::Setter<TargType, ArgType>>( \
                    [](TargType& targ, ArgType val) \
                    { \
                        targ.setter(val); \
                    })); \
    } \
}; \
struct Prop_ ## name { \
    Prop_ ## name() \
    { \
        static InitProp_ ## name _; \
    } \
} prop_ ## name

/// Use member functions as a property getters and setters.
#define SGT_PROP_GET_SET(name, getter, RetType, setter, ArgType) \
struct InitProp_ ## name \
{ \
    InitProp_ ## name() \
    { \
        sPropertyMap()[#name] = \
            std::make_shared<Sgt::Property<RetType, ArgType>>( \
                std::make_unique<Sgt::Getter<TargType, RetType>>( \
                    [](const TargType& targ)->RetType \
                    { \
                        return targ.getter(); \
                    }), \
                std::make_unique<Sgt::Setter<TargType, ArgType>>( \
                    [](TargType& targ, ArgType val) \
                    { \
                        targ.setter(val); \
                    })); \
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
