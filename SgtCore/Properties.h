#ifndef PROPERTIES_DOT_H
#define PROPERTIES_DOT_H

#include <SgtCore/YamlSupport.h>

#include<map>
#include<memory>
#include<stdexcept>
#include<string>
#include<sstream>
#include<vector>

#define SGT_INIT_PROPERTIES(Targ) virtual std::map<std::string, std::shared_ptr<PropertyBase>>& map() override {return HasProperties<Targ>::sMap();}

#define SGT_INHERIT_PROPERTIES(Targ, Base) SGT_INIT_PROPERTIES(Targ); struct Inherit {Inherit(){auto& targMap = HasProperties<Targ>::sMap(); auto& baseMap = HasProperties<Base>::sMap(); for (auto& elem : baseMap) {targMap[elem.first] = elem.second;}}}; struct DoInherit {DoInherit(){static Inherit _;}} inherit

#define SGT_PROPERTY_GET(name, Targ, T, GetBy, getter) struct InitProp_ ## name {InitProp_ ## name(){HasProperties<Targ>::sMap()[#name] = std::make_shared<Property<T, GetBy>>(std::unique_ptr<Getter<Targ, T, GetBy>>(new Getter<Targ, T, GetBy>([](const Targ& targ)->T{return targ.getter();})), nullptr);}}; struct Prop_ ## name {Prop_ ## name(){static InitProp_ ## name _;}} prop_ ## name;

#define SGT_PROPERTY_SET(name, Targ, T, setter) struct InitProp_ ## name {InitProp_ ## name(){HasProperties<Targ>::sMap()[#name] = std::make_shared<Property<T, GetByNone>>(std::unique_ptr<Setter<Targ, T>>(nullptr, new Setter<Targ, T>([](Targ& targ, const T& val){targ.setter();})));}}; struct Prop_ ## name {Prop_ ## name(){static InitProp_ ## name _;}} prop_ ## name

#define SGT_PROPERTY_GET_SET(name, Targ, T, GetBy, getter, setter) struct InitProp_ ## name {InitProp_ ## name(){HasProperties<Targ>::sMap()[#name] = std::make_shared<Property<<T, GetBy>>(std::unique_ptr<Getter<Targ, T, GetBy>>(new Getter<Targ, T, GetBy>([](const Targ& targ)->T{return targ.name();})), std::unique_ptr<Setter<Targ, T>>(new Setter<Targ, T>([](Targ& targ, const T& val){targ.setter();})));}}; struct Prop_ ## name {Prop_ ## name(){static InitProp_ ## name _;}} prop_ ## name

namespace SmartGridToolbox
{
   class PropertyBase;

   class HasPropertiesInterface
   {
      virtual std::map<std::string, std::shared_ptr<PropertyBase>>& map() = 0;
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
         NotGettableException() : std::logic_error("Property is not gettable") {};
   };

   class NotSettableException : public std::logic_error
   {
      public:
         NotSettableException() : std::logic_error("Property is not settable") {};
   };
   
   class BadTargException : public std::logic_error
   {
      public:
         BadTargException() : std::logic_error("Target supplied to property has the wrong type") {};
   };
   
   template<class T> using GetByConstRef = const T&;
   template<class T> using GetByVal = T;
   template<class T> struct GetByNone {}; // Used if there is no getter. 

   template<typename T, template<typename> class GetBy> class GetterInterface
   {
      public:
         virtual GetBy<T> get(const HasPropertiesInterface& targ) const = 0;
   };

   template<typename Targ, typename T, template<typename> class GetBy>
   class Getter : public GetterInterface<T, GetBy>
   {
      public:
         using Get = GetBy<T> (const Targ&);

         Getter(Get getArg) : get_(getArg)
         {
            // Empty.
         }

         virtual GetBy<T> get(const HasPropertiesInterface& targ) const override
         {
            auto derived = dynamic_cast<const Targ*>(&targ);
            if (derived == nullptr)
            {
               throw BadTargException();
            }
            return get_(*derived);
         }

         GetBy<T> get(const Targ& targ) const
         {
            return get_(targ);
         }

      private:
         std::function<Get> get_;
   };
    
   template<typename T> class SetterInterface
   {
      public:
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
            auto derived = dynamic_cast<Targ&>(targ);
            if (derived == nullptr)
            {
               throw BadTargException();
            }
            set_(derived, val);
         }

         void set(Targ& targ, const T& val) const
         {
            set_(targ, val);
         }

      private:
         std::function<Set> set_;
   };

   template<typename T> class PropertyBase1;
   template<typename T, template<typename> class GetBy> class Property;

   class PropertyBase
   {
      public:
         virtual ~PropertyBase() = default;

         virtual bool isGettable() = 0;

         virtual bool isSettable() = 0;

         template<typename T, template<typename> class GetBy> GetBy<T> get(const HasPropertiesInterface& targ) const
         {
            auto derived = dynamic_cast<const Property<T, GetBy>*>(this);
            return derived.template get<GetBy>(targ);
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
            set(targ, fromYamlString<T>(str)); 
         }

      private:
         std::unique_ptr<SetterInterface<T>> setter_;
   };
   
   template<typename T, template<typename> class GetBy> class Property : public PropertyBase1<T>
   {
      public:
         Property(std::unique_ptr<GetterInterface<T, GetBy>> getter, std::unique_ptr<SetterInterface<T>> setter) :
            PropertyBase1<T>(std::move(setter)),
            getter_(std::move(getter))
         {
            // Empty.
         }
         
         virtual bool isGettable() override
         {
            return getter_ != nullptr;
         }
         
         GetBy<T> get(const HasPropertiesInterface& targ) const
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
         std::unique_ptr<GetterInterface<T, GetBy>> getter_;
   };
}

#endif // PROPERTIES_DOT_H
