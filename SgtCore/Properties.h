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
   
   template<typename Targ, typename T> class GetterTemplate
   {
      public:
         using Get = T (const Targ&);

         GetterTemplate(Get getArg) : get_(getArg)
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
   
   template<typename Targ, typename T> class Getter
   {
      public:
         using Get = T ();

         Getter(const GetterTemplate<Targ, T>* getterTemplate, const Targ* targ) :
            getterTemplate_(getterTemplate), targ_(targ)
         {
            // Empty.
         }

         T get() const
         {
            return getterTemplate_->get(*targ_);
         }

      private:
         const GetterTemplate<Targ, T>* getterTemplate_;
         const Targ* targ_;
   };
   
   template<typename Targ, typename T> class SetterTemplate
   {
      public:
         using Set = void (Targ&, const T&);

         SetterTemplate(Set setArg) : set_(setArg)
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
   
   template<typename Targ, typename T> class Setter
   {
      public:
         using Set = void (const T&);

         Setter(const SetterTemplate<Targ, T>* setterTemplate, Targ* targ) :
            setterTemplate_(setterTemplate), targ_(targ)
         {
            // Empty.
         }

         void set(const T& val)
         {
            setterTemplate_->set(*targ_, val);
         }

      private:
         const SetterTemplate<Targ, T>* setterTemplate_;
         Targ* targ_;
   };

   template<typename T> class Property
   {
      public:
         virtual ~Property() = default;
         virtual T get() const = 0;
         virtual void set(const T& val) = 0;
   };
  
   class PropBase
   {
      public:
         virtual ~PropBase() = default;

         template<typename T> T get() const
         {
            auto propT = dynamic_cast<const Property<T>>(this);
            if (propT == nullptr)
            {
               throw NoGetterException();
            }
            return propT->get();
         }

         template<typename T> void set(const T& val)
         {
            auto propT = dynamic_cast<Property<T>>(this);
            if (propT == nullptr)
            {
               throw NoSetterException();
            }
            propT->set(val);
         }
   };
   
   template<typename Targ, typename T> class TargProp : public Property<T>, public PropBase
   {
      public:
         TargProp(const Getter<Targ, T>* getter, Setter<Targ, T>* setter) :
            getter_(getter), setter_(setter)
         {
            // Empty.
         }

         virtual ~TargProp()
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
            return getter_->get();
         }

         virtual void set(const T& val) override
         {
            if (setter_ == nullptr)
            {
               throw NoSetterException();
            }
            setter_->set(val);
         }

      private:
         const Getter<Targ, T>* getter_;
         Setter<Targ, T>* setter_;
   };

   class HasProperties;

   class PropTemplateBase
   {
      public:
         virtual ~PropTemplateBase() = default;

         virtual std::unique_ptr<const PropBase> baseBind(const HasProperties* targ) const
         {
            return nullptr;
         }

         virtual std::unique_ptr<PropBase> baseBind(HasProperties* targ) const
         {
            return nullptr;
         }
   };

   template<typename Targ, typename T> class PropTemplate : public PropTemplateBase
   {
      public:
         PropTemplate(typename GetterTemplate<Targ, T>::Get getArg) :
            getterTemplate_(new GetterTemplate<Targ, T>(getArg))
         {
            // Empty.
         }

         PropTemplate(typename SetterTemplate<Targ, T>::Set setArg) :
            setterTemplate_(new SetterTemplate<Targ, T>(setArg))
         {
            // Empty.
         }
         
         PropTemplate(typename GetterTemplate<Targ, T>::Get getArg, typename SetterTemplate<Targ, T>::Set setArg) :
            getterTemplate_(new GetterTemplate<Targ, T>(getArg)),
            setterTemplate_(new SetterTemplate<Targ, T>(setArg))
         {
            // Empty.
         }

         virtual ~PropTemplate()
         {
            delete getterTemplate_;
            delete setterTemplate_;
         }

         T get(const Targ& targ) const
         {
            return getterTemplate_->get(targ);
         }
         
         void set(Targ& targ, const T& val) const
         {
            setterTemplate_->set(targ, val);
         }
         
         virtual std::unique_ptr<const PropBase> baseBind(const HasProperties* targ) const override
         {
            const Targ* derived = dynamic_cast<const Targ*>(targ);
            return std::unique_ptr<TargProp<Targ, T>>(new TargProp<Targ, T>(
                     new Getter<Targ, T>(getterTemplate_, derived),
                     nullptr));
         }
         
         virtual std::unique_ptr<PropBase> baseBind(HasProperties* targ) const override
         {
            Targ* derived = dynamic_cast<Targ*>(targ);
            return std::unique_ptr<TargProp<Targ, T>>(new TargProp<Targ, T>(
                     new Getter<Targ, T>(getterTemplate_, derived),
                     new Setter<Targ, T>(setterTemplate_, derived)));
         }
         
         std::unique_ptr<Property<T>> bind(const Targ* targ) const
         {
            return std::unique_ptr<TargProp<Targ, T>>(new TargProp<Targ, T>(
                     new Getter<Targ, T>(getterTemplate_, targ),
                     nullptr));
         }
         
         std::unique_ptr<TargProp<Targ, T>> bind(Targ* targ) const
         {
            return std::unique_ptr<TargProp<Targ, T>>(new TargProp<Targ, T>(
                     new Getter<Targ, T>(getterTemplate_, targ),
                     new Setter<Targ, T>(setterTemplate_, targ)));
         }

      private:
         GetterTemplate<Targ, T>* getterTemplate_{nullptr};
         SetterTemplate<Targ, T>* setterTemplate_{nullptr};
   };

   class PropertiesBase
   {
      public:
         ~PropertyTemplates()
         {
            for (auto item : map)
            {
               delete item.second;
            }
         }

         static std::map<std::string, const PropTemplateBase*> sMap;
   };

   template<typename Targ> class Properties : private PropertiesBase
   {
      public:

         /// Constructor for static initialization.
         /// 
         /// Meant to be implemented with Targ implementation, used to add properties to a class.
         Properties();

         template<typename T>
         void add(const std::string& key, typename GetterTemplate<Targ, T>::Get getArg)
         {
            PropertiesBase::sMap[key] = new PropTemplate<Targ, T>(getArg);
         }

         template<typename T>
         void add(const std::string& key, typename SetterTemplate<Targ, T>::Set setArg)
         {
            PropertiesBase::sMap[key] = new PropTemplate<Targ, T>(setArg);
         }

         template<typename T>
         void add(const std::string& key, 
                  typename GetterTemplate<Targ, T>::Get getArg, typename SetterTemplate<Targ, T>::Set setArg)
         {
            PropertiesBase::sMap[key] = new PropTemplate<Targ, T>(getArg, setArg);
         }
   };

   class HasProperties
   {
      public:
         virtual ~HasProperties() = default;

         template<typename Targ, typename T>
         static void addProperty(const std::string& key, typename GetterTemplate<Targ, T>::Get getArg)
         {
            props().add<Targ, T>(key, getArg);
         }

         template<typename Targ, typename T>
         static void addProperty(const std::string& key, typename SetterTemplate<Targ, T>::Set setArg)
         {
            props().add<Targ, T>(key, setArg);
         }

         template<typename Targ, typename T>
         static void addProperty(const std::string& key, 
               typename GetterTemplate<Targ, T>::Get getArg, typename SetterTemplate<Targ, T>::Set setArg)
         {
            props().add<Targ, T>(key, getArg, setArg);
         }
         
         std::unique_ptr<const PropBase> property(const std::string& key) const
         {
            return property_<PropBase>(key);
         }

         std::unique_ptr<PropBase> property(const std::string& key)
         {
            return property_<PropBase>(key);
         }

         template<typename T> std::unique_ptr<const Property<T>> property(const std::string& key) const
         {
            return property_<Property<T>>(key);
         };

         template<typename T> std::unique_ptr<Property<T>> property(const std::string& key)
         {
            return property_<Property<T>>(key);
         };
         
         std::map<std::string, std::unique_ptr<const PropBase>> properties() const
         {
            return properties_<PropBase>();
         }

         std::map<std::string, std::unique_ptr<PropBase>> properties()
         {
            return properties_<PropBase>();
         }
         
         template<typename T> std::map<std::string, std::unique_ptr<const Property<T>>> properties() const
         {
            return properties_<Property<T>>();
         }
         
         template<typename T> std::map<std::string, std::unique_ptr<Property<T>>> properties()
         {
            return properties_<Property<T>>();
         }
         
      private:

         virtual void initProperties()
         {
            // Empty.
         };

         static PropertyTemplates& props()
         {
            static PropertyTemplates sProps;
            return sProps;
         }

         template<typename PropType> std::unique_ptr<const PropType> property_(const std::string& key) const
         {
            std::unique_ptr<const PropType> result = nullptr;
            auto it = props().map.find(key);
            if (it != props().map.end())
            {
               auto prop = it->second->baseBind(this);
               result.reset(dynamic_cast<const PropType*>(prop.release()));
            }
            return result;
         }

         template<typename PropType> std::unique_ptr<PropType> property_(const std::string& key)
         {
            std::unique_ptr<PropType> result = nullptr;
            auto it = props().map.find(key);
            if (it != props().map.end())
            {
               auto prop = it->second->baseBind(this);
               result.reset(dynamic_cast<PropType*>(prop.release()));
            }
            return result;
         }

         template<typename PropType>
         std::map<std::string, std::unique_ptr<const PropType>> properties_() const
         {
            std::map<std::string, std::unique_ptr<const PropType>> result;
            for (auto& elem : props().map)
            {
               auto prop = elem.second->baseBind(this);
               if (prop != nullptr)
               {
                  result[elem.first] = std::move(prop);
               }
            }
            return result;
         }

         template<typename PropType>
         std::map<std::string, std::unique_ptr<PropType>> properties_()
         {
            std::map<std::string, std::unique_ptr<PropType>> result;
            for (auto& elem : props().map)
            {
               auto prop = elem.second->baseBind(this);
               if (prop != nullptr)
               {
                  result[elem.first] = std::move(prop);
               }
            }
            return result;
         }
   };
}

#endif // PROPERTIES_DOT_H
