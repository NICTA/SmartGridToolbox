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

#include <SgtCore/YamlSupport.h>

#include<map>
#include<memory>
#include<stdexcept>
#include<string>
#include<sstream>
#include<type_traits>
#include<vector>

#define SGT_PROPS_INIT(Targ) virtual const std::map<std::string, std::shared_ptr<PropertyBase>>& properties() const override {return HasProperties<Targ>::sMap();}; virtual std::map<std::string, std::shared_ptr<PropertyBase>>& properties() override {return HasProperties<Targ>::sMap();}

#define SGT_PROPS_INHERIT(Targ, Base) struct Inherit ## Base {Inherit ## Base (){auto& targMap = HasProperties<Targ>::sMap(); auto& baseMap = HasProperties<Base>::sMap(); for (auto& elem : baseMap) {targMap[elem.first] = elem.second;}}}; struct DoInherit ## Base {DoInherit ## Base(){static Inherit ## Base inherit ## Base;}} doinherit ## Base

#define SGT_PROP_GET(name, Targ, T, getter) struct InitProp_ ## name {InitProp_ ## name(){HasProperties<Targ>::sMap()[#name] = std::make_shared<Property<T>>(std::unique_ptr<Getter<Targ, T>>(new Getter<Targ, T>([](const Targ& targ)->T{return targ.getter();})), nullptr);}}; struct Prop_ ## name {Prop_ ## name(){static InitProp_ ## name _;}} prop_ ## name;

#define SGT_PROP_SET(name, Targ, T, setter) struct InitProp_ ## name {InitProp_ ## name(){HasProperties<Targ>::sMap()[#name] = std::make_shared<Property<T>>(std::unique_ptr<Setter<Targ, T>>(nullptr, new Setter<Targ, T>([](Targ& targ, const T& val){targ.setter(val);})));}}; struct Prop_ ## name {Prop_ ## name(){static InitProp_ ## name _;}} prop_ ## name

#define SGT_PROP_GET_SET(name, Targ, T, getter, setter) struct InitProp_ ## name {InitProp_ ## name(){HasProperties<Targ>::sMap()[#name] = std::make_shared<Property<T>>(std::unique_ptr<Getter<Targ, T>>(new Getter<Targ, T>([](const Targ& targ)->T{return targ.name();})), std::unique_ptr<Setter<Targ, T>>(new Setter<Targ, T>([](Targ& targ, const T& val){targ.setter(val);})));}}; struct Prop_ ## name {Prop_ ## name(){static InitProp_ ## name _;}} prop_ ## name

namespace Sgt
{
    template<typename T> using TypeByVal = typename std::remove_const<typename std::remove_reference<T>::type>::type;

    class PropertyBase;

    class HasPropertiesInterface
    {
        public:
            virtual ~HasPropertiesInterface() = default;
            virtual const std::map<std::string, std::shared_ptr<PropertyBase>>& properties() const = 0;
            virtual std::map<std::string, std::shared_ptr<PropertyBase>>& properties() = 0;
    };

    template<typename Targ> class HasProperties : virtual public HasPropertiesInterface
    {
        public:
            static std::map<std::string, std::shared_ptr<PropertyBase>>& sMap()
            {
                static std::map<std::string, std::shared_ptr<PropertyBase>> map;
                return map;
            }
    };

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

    template<typename T> class GetterInterface
    {
        public:
            virtual ~GetterInterface() = default;
            virtual T get(const HasPropertiesInterface& targ) const = 0;
    };

    template<typename Targ, typename T> class Getter : public GetterInterface<T>
    {
        public:
            using Get = T (const Targ&);

            Getter(Get getArg) : get_(getArg)
            {
                // Empty.
            }

            virtual T get(const HasPropertiesInterface& targ) const override
            {
                auto derived = dynamic_cast<const Targ*>(&targ);
                if (derived == nullptr)
                {
                    throw BadTargException();
                }
                return get_(*derived);
            }

            T get(const Targ& targ) const
            {
                return get_(targ);
            }

        private:
            std::function<Get> get_;
    };

    template<typename T> class SetterInterface
    {
        public:
            virtual ~SetterInterface() = default;
            using Set = void (HasPropertiesInterface&, const T&);
            virtual void set(HasPropertiesInterface& targ, const T& val) const = 0;
    };

    template<typename Targ, typename T>
    class Setter : public SetterInterface<T>
    {
        public:
            using Set = void (Targ&, const T&);

            Setter(Set setArg) : set_(setArg)
            {
                // Empty.
            }

            virtual void set(HasPropertiesInterface& targ, const T& val) const override
            {
                auto derived = dynamic_cast<Targ*>(&targ);
                if (derived == nullptr)
                {
                    throw BadTargException();
                }
                set_(*derived, val);
            }

            void set(Targ& targ, const T& val) const
            {
                set_(targ, val);
            }

        private:
            std::function<Set> set_;
    };

    template<typename T> class PropertyBase1;
    template<typename T> class Property;

    class PropertyBase
    {
        public:
            virtual ~PropertyBase() = default;

            virtual bool isGettable() = 0;

            virtual bool isSettable() = 0;

            template<typename T> T get(const HasPropertiesInterface& targ) const
            {
                auto derived = dynamic_cast<const Property<T>*>(this);
                return derived.get(targ);
            }

            template<typename T> void set(HasPropertiesInterface& targ, const T& val) const
            {
                auto derived = dynamic_cast<const PropertyBase1<T>*>(this);
                derived.set(targ, val);
            }

            virtual std::string string(const HasPropertiesInterface& targ) = 0;

            virtual void setFromString(HasPropertiesInterface& targ, const std::string& str) = 0;
    };

    template<typename T> class PropertyBase1 : public PropertyBase
    {
        public:

            PropertyBase1(std::unique_ptr<SetterInterface<T>> setter) :
                setter_(std::move(setter))
            {
                // Empty.
            }

            virtual bool isSettable() override
            {
                return setter_ != nullptr;
            }

            void set(HasPropertiesInterface& targ, const T& val) const
            {
                if (setter_ == nullptr)
                {
                    throw NotSettableException();
                }
                setter_->set(targ, val);
            }

            virtual void setFromString(HasPropertiesInterface& targ, const std::string& str) override
            {
                set(targ, fromYamlString<TypeByVal<T>>(str));
            }

        private:
            std::unique_ptr<SetterInterface<T>> setter_;
    };

    /// @brief Dynamically discoverable property.
    ///
    /// A property can have a getter and a setter. It can generically get its value and set via strings,
    /// or can get and set its the particular template type, if we know what that is.
    /// @ingroup Core
    template<typename T> class Property : public PropertyBase1<T>
    {
        public:
            Property(std::unique_ptr<GetterInterface<T>> getter, std::unique_ptr<SetterInterface<T>> setter) :
                PropertyBase1<T>(std::move(setter)),
                getter_(std::move(getter))
            {
                // Empty.
            }

            virtual bool isGettable() override
            {
                return getter_ != nullptr;
            }

            T get(const HasPropertiesInterface& targ) const
            {
                if (getter_ == nullptr)
                {
                    throw NotGettableException();
                }
                return getter_->get(targ);
            }

            virtual std::string string(const HasPropertiesInterface& targ)
            {
                return toYamlString(get(targ));
            }

        private:
            std::unique_ptr<GetterInterface<T>> getter_;
    };
}

#endif // PROPERTIES_DOT_H
