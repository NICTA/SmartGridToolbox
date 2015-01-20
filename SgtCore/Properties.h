#ifndef PROPERTIES_DOT_H
#define PROPERTIES_DOT_H

#include <SgtCore/YamlSupport.h>

#include<map>
#include<memory>
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

   template<typename Targ, typename T, bool hasGetter, bool hasSetter> class PropertyTemplate;

   class PropertyTemplateBase
   {
      public:
         virtual ~PropertyTemplateBase() = default;
   };

   template<typename Targ, typename T>
   class PropertyTemplate<Targ, T, true, false> : virtual public PropertyTemplateBase
   {
      protected:
         PropertyTemplate(Get<Targ, T> getArg) : get_(getArg)
         {
            // Empty.
         }

         std::function<Get<Targ, T>> get_;
   };
   
   template<typename Targ, typename T>
   class PropertyTemplate<Targ, T, false, true> : virtual public PropertyTemplateBase 
   {
      friend class Property<T, false, true>;

      protected:
         PropertyTemplate(Set<Targ, T> setArg) : set_(setArg)
         {
            // Empty.
         }

         std::function<Set<Targ, T>> set_;
   };
   
   template<typename Targ, typename T> class PropertyTemplate<Targ, T, true, true> : 
   virtual public PropertyTemplate<Targ, T, true, false>, virtual public PropertyTemplate<Targ, T, false, true>
   {
      protected:
         PropertyTemplate(Get<Targ, T> getArg, Set<Targ, T> setArg) :
            PropertyTemplate<Targ, T, true, false>(getArg),
            PropertyTemplate<Targ, T, false, true>(setArg)
         {
            // Empty.
         }
   };

   template<typename Targ, typename T, bool isGettable, bool isSettable> class TargetProperty;

   template<typename Targ, typename T>
   class TargetProperty<Targ, T, true, false> : virtual public Property<T, true, false>
   {
      public:
         TargetProperty(const Targ* targ, const PropertyTemplate<Targ, T, true, false>* propTemplate) : 
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
         const PropertyTemplate<Targ, T, true, false>* propTemplate_;
   };

   template<typename Targ, typename T>
   class TargetProperty<Targ, T, false, true> : virtual public Property<T, false, true>
   {
      public:
         TargetProperty(Targ* targ, const PropertyTemplate<Targ, T, true, false>* propTemplate) : 
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
         const PropertyTemplate<Targ, T, true, false>* propTemplate_;
   };

   template<typename Targ, typename T>
   class TargetProperty<Targ, T, true, true> : virtual public Property<T, true, true>
   {
      public:
         TargetProperty(Targ* targ, const PropertyTemplate<Targ, T, true, true>* propTemplate) : 
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
         const PropertyTemplate<Targ, T, true, false>* propTemplate_;
   };

   template<typename Targ> class HasProperties
   {
      public:

         template<typename T> static void addProperty(const std::string& key, Get<Targ, T> getArg)
         {
            map_[key] = new PropertyTemplate<Targ, T, true, false>(getArg);
         }

         template<typename T> static void addProperty(const std::string& key, Set<Targ, T> setArg)
         {
            map_[key] = new PropertyTemplate<Targ, T, false, true>(setArg);
         }

         template<typename T>
         static void addProperty(const std::string& key, Get<Targ, T> getArg, Set<Targ, T> setArg)
         {
            map_[key] = new PropertyTemplate<Targ, T, true, true>(getArg, setArg);
         }

         std::unique_ptr<PropertyBase> property(const std::string& key)
         {
            auto it = map_.find(key);
            return (it != map_.end())
               ? std::unique_ptr<PropertyBase>(
                     new TargetProperty<Targ, T, isGettable, isSettable>(
                        dynamic_cast<const Targ*>(this),
                        dynamic_cast<const PropertyTemplate<Targ, T, isGettable, isSettable>*>(it->second)))
               : std::unique_ptr<Property<T, isGettable, isSettable>>(nullptr);
         }

         template<typename T, bool isGettable, bool isSettable>
         std::unique_ptr<const Property<T, isGettable, isSettable>> property(const std::string& key) const
         {
            auto it = map_.find(key);
            return (it != map_.end())
               ? std::unique_ptr<Property<T, isGettable, isSettable>>(
                     new TargetProperty<Targ, T, isGettable, isSettable>(
                        dynamic_cast<const Targ*>(this),
                        dynamic_cast<const PropertyTemplate<Targ, T, isGettable, isSettable>*>(it->second)))
               : std::unique_ptr<Property<T, isGettable, isSettable>>(nullptr);
         }

         template<typename T, bool isGettable, bool isSettable>
         std::unique_ptr<Property<T, isGettable, isSettable>> property(const std::string& key)
         {
            auto it = map_.find(key);
            return (it != map_.end())
               ? std::unique_ptr<Property<T, isGettable, isSettable>>(
                     new TargetProperty<Targ, T, isGettable, isSettable>(
                        dynamic_cast<const Targ*>(this),
                        dynamic_cast<const PropertyTemplate<Targ, T, isGettable, isSettable>*>(it->second)))
               : std::unique_ptr<Property<T, isGettable, isSettable>>(nullptr);
         }

      private:

         HasProperties();

         ~HasProperties()
         {
            for (auto& p : map_)
            {
               delete p.second;
            }
         }

      private:
         static std::map<std::string, PropertyTemplateBase*> map_;
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
