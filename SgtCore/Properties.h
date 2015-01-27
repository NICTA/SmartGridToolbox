#ifndef PROPERTIES_DOT_H
#define PROPERTIES_DOT_H

#include <SgtCore/YamlSupport.h>

#include<map>
#include<memory>
#include<stdexcept>
#include<string>
#include<sstream>
#include<vector>

#define SGT_PROP_GET(name, Targ, T, GetBy, get) Property<Targ, T, GetBy> name##_Prop_{#name, this, [](const Targ& targ)->GetBy<T>{return targ.get();}}
#define SGT_PROP_SET(name, Targ, T, set) Property<Targ, T, GetByNone> name##_Prop_{#name, this, [](Targ& targ, const T& val){targ.set(val);}}
#define SGT_PROP_GET_SET(name, Targ, T, GetBy, get, set) Property<Targ, T, GetBy> name##_Prop_{#name, this, [](const Targ& targ)->GetBy<T>{return targ.get();}, [](Targ& targ, const T& val){targ.set(val);}}

namespace SmartGridToolbox
{
   class NoGetterException : public std::logic_error
   {
      public:
         NoGetterException() : std::logic_error("Property does not have a getter") {};
   };
   
   class NoSetterException : public std::logic_error
   {
      public:
         NoSetterException() : std::logic_error("Property does not have a setter") {};
   };
   
   class BadTargetException : public std::logic_error
   {
      public:
         BadTargetException() : std::logic_error("Property has the wrong target type for its target object") {};
   };
   
   class BadGetByException : public std::logic_error
   {
      public:
         BadGetByException() : std::logic_error("Property has the wrong get by type") {};
   };

   template<class T> using GetByConstRef = const T&;
   template<class T> using GetByVal = T;
   template<class T> struct GetByNone {}; // Used if there is no getter. 

   template<typename Targ, typename T, template<typename> class GetBy> class Getter
   {
      public:
         using Get = GetBy<T> (const Targ&);

         Getter(Get getArg) : get_(getArg)
         {
            // Empty.
         }

         GetBy<T> get(const Targ& targ) const
         {
            return get_(targ);
         }

      private:
         std::function<Get> get_;
   };
   
   template<typename Targ, typename T> class Setter
   {
      public:
         using Set = void (Targ&, const T&);

         Setter(Set setArg) : set_(setArg)
         {
            // Empty.
         }

         void set(Targ& targ, const T& val) const
         {
            set_(targ, val);
         }

      private:
         std::function<Set> set_;
   };
   
   template<typename T, template<typename> class GetBy> class PropertyGetterInterface
   {
      public:
         virtual ~PropertyGetterInterface() = default;
         virtual GetBy<T> get() const = 0;
   };
   
   template<typename T> class PropertySetterInterface
   {
      public:
         virtual ~PropertySetterInterface() = default;
         virtual void set(const T& val) = 0;
   };

   class PropCommon
   {
      public:
         virtual ~PropCommon() = default;

         template<typename T, template<typename> class GetBy> GetBy<T> get() const
         {
            auto propT = dynamic_cast<const PropertyGetterInterface<T, GetBy>>(this);
            if (propT == nullptr)
            {
               throw NoGetterException();
            }
            return propT->get();
         }

         template<typename T> void set(const T& val)
         {
            auto propT = dynamic_cast<PropertySetterInterface<T>>(this);
            if (propT == nullptr)
            {
               throw NoSetterException();
            }
            propT->set(val);
         }
   };
   
   template<typename Targ, typename T, template<typename> class GetBy>
   class Property : public PropCommon, public PropertyGetterInterface<T, GetBy>, public PropertySetterInterface<T>
   {
      public:
         Property(const std::string& key, Targ* targ, typename Getter<Targ, T, GetBy>::Get getArg) :
            targ_(targ),
            getter_(new Getter<Targ, T, GetBy>(getArg))
         {
            targ->addProperty(key, this);
         }
         
         Property(const std::string& key, Targ* targ, typename Setter<Targ, T>::Set setArg) :
            targ_(targ),
            setter_(new Setter<Targ, T>(setArg))
         {
            if (!std::is_same<GetBy<T>, GetByNone<T>>())
            {
               throw BadGetByException();
            };

            targ->addProperty(key, this);
         }
         
         Property(const std::string& key, Targ* targ,
               typename Getter<Targ, T, GetBy>::Get getArg, typename Setter<Targ, T>::Set setArg) :
            targ_(targ),
            getter_(new Getter<Targ, T, GetBy>(getArg)),
            setter_(new Setter<Targ, T>(setArg))
         {
            targ->addProperty(key, this);
         }

         virtual ~Property()
         {
            delete getter_;
            delete setter_;
         }

         virtual GetBy<T> get() const override
         {
            if (getter_ == nullptr)
            {
               throw NoGetterException();
            }
            return getter_->get(*targ_);
         }

         virtual void set(const T& val) override
         {
            if (setter_ == nullptr)
            {
               throw NoSetterException();
            }
            setter_->set(*targ_, val);
         }

      private:
         Targ* targ_;
         Getter<Targ, T, GetBy>* getter_{nullptr};
         Setter<Targ, T>* setter_{nullptr};
   };

   class HasPropertiesInterface
   {
      public:
         using PropMap = std::map<std::string, PropCommon*>;
         virtual ~HasPropertiesInterface() = default;
         virtual const PropCommon* property(const std::string& key) const = 0;
         virtual PropCommon* property(const std::string& key) = 0;
         virtual PropMap::iterator propertiesBegin() = 0;
         virtual PropMap::iterator propertiesEnd() = 0;
         virtual PropMap::const_iterator propertiesBegin() const = 0;
         virtual PropMap::const_iterator propertiesEnd() const = 0;
   };

   class HasProperties : virtual public HasPropertiesInterface
   {
      template<typename Targ, typename T, template<typename> class GetBy> friend class Property;

      public:
         virtual ~HasProperties() = default;

         virtual const PropCommon* property(const std::string& key) const override
         {
            return property_<PropCommon>(key);
         }

         virtual PropCommon* property(const std::string& key) override
         {
            return property_<PropCommon>(key);
         }

         virtual PropMap::iterator propertiesBegin() override
         {
            return map_.begin();
         }
         
         virtual PropMap::iterator propertiesEnd() override
         {
            return map_.end();
         }

         virtual PropMap::const_iterator propertiesBegin() const override
         {
            return map_.cbegin();
         }
         
         virtual PropMap::const_iterator propertiesEnd() const override
         {
            return map_.cend();
         }
         
      private:
         template<typename Targ, typename T, template<typename> class GetBy>
         void addProperty(const std::string& key, Property<Targ, T, GetBy>* prop)
         {
            map_[key] = prop;
         }

         template<typename PropType> const PropType* property_(const std::string& key) const
         {
            const PropType* result = nullptr;
            auto it = map_.find(key);
            if (it != map_.end())
            {
               auto prop = it->second;
               result = dynamic_cast<const PropType*>(prop);
            }
            return result;
         }

         template<typename PropType> PropType* property_(const std::string& key)
         {
            PropType* result = nullptr;
            auto it = map_.find(key);
            if (it != map_.end())
            {
               auto prop = it->second;
               result = dynamic_cast<PropType*>(prop);
            }
            return result;
         }

      private:
         std::map<std::string, PropCommon*> map_;
   };
}

#endif // PROPERTIES_DOT_H
