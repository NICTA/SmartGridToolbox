#ifndef PROPERTIES_DOT_H
#define PROPERTIES_DOT_H

#include <SgtCore/YamlSupport.h>

#include<map>
#include<stdexcept>
#include<string>
#include<sstream>
#include<vector>

namespace SmartGridToolbox
{
   template<typename Targ, typename T> using Get = T (const Targ&);
   template<typename Targ, typename T> using Set = void (const Targ&, const T&);

   template<typename T, bool hasGetter, bool hasSetter> class Property;

   template<typename T, bool isGettable, bool isSettable> class Property;

   class PropertyBase
   {
      public:
         virtual ~PropertyBase() = default;

         virtual operator bool()
         {
            return false;
         }

         virtual bool isGettable()
         {
            return false;
         }

         virtual bool isSettable()
         {
            return false;
         }

         template<typename T, bool isGettable, bool isSettable>
         const Property<T, isGettable, isSettable> ofType() const
         {
            return dynamic_cast<const Property<T, isGettable, isSettable>>(this);
         }

         template<typename T, bool isGettable, bool isSettable> Property<T, isGettable, isSettable> ofType()
         {
            return dynamic_cast<Property<T, isGettable, isSettable>>(this);
         }

         virtual std::string string()
         {
            throw std::runtime_error("Property is not gettable");
         }

         virtual void setFromString(const std::string& str)
         {
            throw std::runtime_error("Property is not settable");
         }
   };
  
   template<typename T> class Property<T, true, false> : virtual public PropertyBase
   {
      public:
         virtual bool isGettable() override
         {
            return true;
         }

         virtual std::string string() override
         {
            return toYamlString(get());
         }

         virtual T get() const = 0;
   };

   template<typename T> class Property<T, false, true> : virtual public PropertyBase
   {
      public:
         virtual bool isSettable() override
         {
            return true;
         }

         virtual void setFromString(const std::string& string) override
         {
            set(fromYamlString<T>(string));
         }

         virtual void set(const T& val) = 0;
   };

   template<typename T>
   class Property<T, true, true> : virtual public Property<T, true, false>, virtual public Property<T, false, true>
   {
      // Empty.
   };

   template<typename Targ, typename T, bool hasGetter, bool hasSetter> class PropTemplate;

   class PropTemplateBase
   {
      public:
         virtual ~PropTemplateBase() = default;
   };

   template<typename Targ, typename T> class PropTemplate<Targ, T, true, false> : virtual public PropTemplateBase
   {
      friend class Properties;

      protected:
         PropTemplate(Get<Targ, T> getArg) : get_(getArg)
         {
            // Empty.
         }

         std::function<Get<Targ, T>> get_;
   };
   
   template<typename Targ, typename T> class PropTemplate<Targ, T, false, true> : virtual public PropTemplateBase 
   {
      friend class Property<T, false, true>;

      protected:
         PropTemplate(Set<Targ, T> setArg) : set_(setArg)
         {
            // Empty.
         }

         std::function<Set<Targ, T>> set_;
   };
   
   template<typename Targ, typename T> class PropTemplate<Targ, T, true, true> : 
      virtual public PropTemplate<Targ, T, true, false>, virtual public PropTemplate<Targ, T, false, true>
   {
      protected:
         PropTemplate(Get<Targ, T> getArg, Set<Targ, T> setArg) :
            PropTemplate<Targ, T, true, false>(getArg),
            PropTemplate<Targ, T, false, true>(setArg)
         {
            // Empty.
         }
   };

   template<typename Targ, typename T, bool isGettable, bool isSettable> class TargProperty;

   template<typename Targ, typename T>
   class TargProperty<Targ, T, true, false> : virtual public Property<T, true, false>
   {
      public:
         TargProperty(const Targ* targ, const PropTemplate<Targ, T, true, false>* propTemplate) : 
            targ_(targ), propTemplate_(propTemplate)
         {
            // Empty.
         }

         virtual T get() const override
         {
            return this->propTemplate_->get(*targ_);
         }

      private:
         const Targ* targ_;
         const PropTemplate<Targ, T, true, false>* propTemplate_;
   };

   template<typename Targ, typename T>
   class TargProperty<Targ, T, false, true> : virtual public Property<T, false, true>
   {
      public:
         TargProperty(Targ* targ, const PropTemplate<Targ, T, true, false>* propTemplate) : 
            targ_(targ), propTemplate_(propTemplate)
         {
            // Empty.
         }

         virtual void set(const T& val) override
         {
            this->propTemplate_->set(*targ_, val);
         }

      private:
         Targ* targ_;
         const PropTemplate<Targ, T, true, false>* propTemplate_;
   };

   template<typename Targ, typename T>
   class TargProperty<Targ, T, true, true> : virtual public Property<T, true, true>
   {
      public:
         TargProperty(Targ* targ, const PropTemplate<Targ, T, true, true>* propTemplate) : 
            targ_(targ), propTemplate_(propTemplate)
         {
            // Empty.
         }

         virtual T get() const override
         {
            return this->propTemplate_->get(*targ_);
         }

         virtual void set(const T& val) override
         {
            this->propTemplate_->set(*targ_, val);
         }
         
      private:
         Targ* targ_;
         const PropTemplate<Targ, T, true, false>* propTemplate_;
   };

   template<typename Targ> class HasProperties
   {
      friend class HasPropertiesBase;

      public:

         template<typename Targ, typename T> void add(const std::string& key, Get<Targ, T> getArg)
         {
            map_[key] = new PropTemplate<Targ, T, true, false>(getArg);
         }

         template<typename Targ, typename T> void add(const std::string& key, Set<Targ, T> setArg)
         {
            map_[key] = new PropTemplate<Targ, T, false, true>(setArg);
         }
         
         template<typename Targ, typename T>
         void add( const std::string& key, Get<Targ, T> getArg, Set<Targ, T> setArg)
         {
            map_[key] = new PropTemplate<Targ, T, true, true>(getArg, setArg);
         }

         template<typename T, bool isGettable, bool isSettable>
         const Property<T, isGettable, isSettable> property(const std::string& key) const
         {
            const Property<T, isGettable, isSettable>* prop = nullptr;
            auto it = map_.find(key);
            if (it != map_.end())
            {
               prop = dynamic_cast<const Property<T, isGettable, isSettable>*>(it->second);
            }
            return prop;
         }

         template<typename T, bool isGettable, bool isSettable>
         const std::map<std::String, Property<T, isGettable, isSettable>* property(const std::string& key) const
         {
            const Property<T, isGettable, isSettable>* prop = nullptr;
            auto it = map_.find(key);
            if (it != map_.end())
            {
               prop = 
            }
            else
            {
               return PropertyBase;
            }
            return prop;
         }

         template<typename T, bool isGettable, bool isSettable>
         Property<T, isGettable, isSettable>* operator[](const std::string& key)
         {
            return const_cast<Property<T, isGettable, isSettable>*>(
                  static_cast<const Properties*>(this)->operator[]<T, isGettable, isSettable>(key));
         }

      private:

         Properties();

         ~Properties()
         {
            for (auto& p : map_)
            {
               delete p.second;
            }
         }

      private:
         std::map<std::string, PropTemplateBase*> map_;
   };

   template<typename Targ> class HasProperties
   {
      public:
         static Properties& properties()
         {
            static Properties sProperties;
            return sProperties;
         }

         virtual ~HasProperties() = default;
   };
}

#endif // PROPERTIES_DOT_H
