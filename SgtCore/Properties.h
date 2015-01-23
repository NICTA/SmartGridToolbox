#ifndef PROPERTIES_DOT_H
#define PROPERTIES_DOT_H

#include <SgtCore/YamlSupport.h>

#include<map>
#include<memory>
#include<stdexcept>
#include<string>
#include<sstream>
#include<vector>

#define SGT_PROP_GET(name, Targ, T, get) Property<Targ, T> name##_Prop{#name, this, [](const Targ& targ){return targ.get();}};
#define SGT_PROP_SET(name, Targ, T, set) Property<Targ, T> name##_Prop{#name, this, [](Targ& targ, const T& val){targ.set(val);}};
#define SGT_PROP_GET_SET(name, Targ, T, get, set) Property<Targ, T> name##_Prop{#name, this, [](const Targ& targ){return targ.get();}, [](Targ& targ, const T& val){targ.set(val);}};

namespace SmartGridToolbox
{
   class NoGetterException : public std::domain_error
   {
      public:
         NoGetterException() : std::domain_error("Property does not have a getter") {};
   };
   
   class NoSetterException : public std::domain_error
   {
      public:
         NoSetterException() : std::domain_error("Property does not have a setter") {};
   };

   class BadTargetException : public std::domain_error
   {
      public:
         BadTargetException() : std::domain_error("Property has the wrong target type for its target object") {};
   };
   
   template<typename Targ, typename T> class Getter
   {
      public:
         using Get = T (const Targ&);

         Getter(Get getArg) : get_(getArg)
         {
            // Empty.
         }

         T get(const Targ& targ) const
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
   
   template<typename T> class PropertyInterface
   {
      public:
         virtual ~PropertyInterface() = default;
         virtual T get() const = 0;
         virtual void set(const T& val) = 0;
   };
  
   class PropCommon
   {
      public:
         virtual ~PropCommon() = default;

         template<typename T> T get() const
         {
            auto propT = dynamic_cast<const PropertyInterface<T>>(this);
            if (propT == nullptr)
            {
               throw NoGetterException();
            }
            return propT->get();
         }

         template<typename T> void set(const T& val)
         {
            auto propT = dynamic_cast<PropertyInterface<T>>(this);
            if (propT == nullptr)
            {
               throw NoSetterException();
            }
            propT->set(val);
         }
   };
   
   template<typename Targ, typename T> class Property : public PropertyInterface<T>, public PropCommon
   {
      public:
         Property(const std::string& key, Targ* targ, typename Getter<Targ, T>::Get getArg) :
            targ_(targ),
            getter_(new Getter<Targ, T>(getArg))
         {
            targ->addProperty(key, this);
         }
         
         Property(const std::string& key, Targ* targ, typename Setter<Targ, T>::Set setArg) :
            targ_(targ),
            setter_(new Setter<Targ, T>(setArg))
         {
            targ->addProperty(key, this);
         }
         
         Property(const std::string& key, Targ* targ,
               typename Getter<Targ, T>::Get getArg, typename Setter<Targ, T>::Set setArg) :
            targ_(targ),
            getter_(new Getter<Targ, T>(getArg)),
            setter_(new Setter<Targ, T>(setArg))
         {
            targ->addProperty(key, this);
         }

         virtual ~Property()
         {
            delete getter_;
            delete setter_;
         }

         virtual T get() const override
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
         Getter<Targ, T>* getter_{nullptr};
         Setter<Targ, T>* setter_{nullptr};
   };

   class HasProperties
   {
      template<typename Targ, typename T> friend class Property;

      public:

         using PropMap = std::map<std::string, PropCommon*>;

         virtual ~HasProperties() = default;

         const PropCommon* property(const std::string& key) const
         {
            return property_<PropCommon>(key);
         }

         PropCommon* property(const std::string& key)
         {
            return property_<PropCommon>(key);
         }

         template<typename T> const PropertyInterface<T>* property(const std::string& key) const
         {
            return property_<PropertyInterface<T>>(key);
         };

         template<typename T> PropertyInterface<T>* property(const std::string& key)
         {
            return property_<PropertyInterface<T>>(key);
         };

         PropMap::iterator propertiesBegin()
         {
            return map_.begin();
         }
         
         PropMap::iterator propertiesEnd()
         {
            return map_.end();
         }

         PropMap::const_iterator propertiesBegin() const
         {
            return map_.cbegin();
         }
         
         PropMap::const_iterator propertiesEnd() const
         {
            return map_.cend();
         }
         
      private:

         template<typename Targ, typename T>
         void addProperty(const std::string& key, Property<Targ, T>* prop)
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
