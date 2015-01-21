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

   template<typename Targ, typename T> class Property;

   template<typename Targ> class PropBase
   {
      public:
         virtual ~PropBase() = default;

         template<typename T> T get() const
         {
            auto derived = dynamic_cast<const Property<Targ, T>>(this);
            if (derived == nullptr)
            {
               throw NoGetterException();
            }
            return derived->get();
         }

         template<typename T> void set(const T& val)
         {
            auto derived = dynamic_cast<Property<Targ, T>>(this);
            if (derived == nullptr)
            {
               throw NoSetterException();
            }
            derived->set(val);
         }
   };
   
   template<typename Targ, typename T> class Property : public PropBase<Targ>
   {
      public:
         Property(const Getter<Targ, T>* getter, Setter<Targ, T>* setter) :
            getter_(getter), setter_(setter)
         {
            // Empty.
         }

         virtual ~Property()
         {
            delete getter_;
            delete setter_;
         }

         T get() const
         {
            if (getter_ == nullptr)
            {
               throw NoGetterException();
            }
            return getter_->get();
         }

         void set(const T& val) const
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

   template<typename Targ> class PropTemplateBase
   {
      public:
         virtual std::unique_ptr<PropBase<const Targ>> baseBind(const Targ* targ)
         {
            return nullptr;
         }

         virtual std::unique_ptr<PropBase<Targ>> baseBind(Targ* targ)
         {
            return nullptr;
         }
   };

   template<typename Targ, typename T> class PropTemplate : public PropTemplateBase<Targ>
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
         
         virtual std::unique_ptr<PropBase<Targ>> baseBind(const Targ* targ) const
         {
            return std::unique_ptr<Property<Targ, T>>(new Property<Targ, T>(
                     new Getter<Targ, T>(getterTemplate_, targ),
                     nullptr));
         }
         
         virtual std::unique_ptr<PropBase<Targ>> baseBind(Targ* targ)
         {
            return std::unique_ptr<Property<Targ, T>>(new Property<Targ, T>(
                     new Getter<Targ, T>(getterTemplate_, targ),
                     new Setter<Targ, T>(setterTemplate_, targ)));
         }
         
         std::unique_ptr<Property<Targ, T>> bind(const Targ* targ) const
         {
            return std::unique_ptr<Property<Targ, T>>(new Property<Targ, T>(
                     new Getter<Targ, T>(getterTemplate_, targ),
                     nullptr));
         }
         
         std::unique_ptr<Property<Targ, T>> bind(Targ* targ)
         {
            return std::unique_ptr<Property<Targ, T>>(new Property<Targ, T>(
                     new Getter<Targ, T>(getterTemplate_, targ),
                     new Setter<Targ, T>(setterTemplate_, targ)));
         }

      private:
         GetterTemplate<Targ, T>* getterTemplate_{nullptr};
         SetterTemplate<Targ, T>* setterTemplate_{nullptr};
   };

   template<typename Targ> class Properties
   {
      public:
         Properties();
         
         ~Properties()
         {
            for (auto item : map)
            {
               delete item.second;
            }
         }

         template<typename T> void add(const std::string& key, typename GetterTemplate<Targ, T>::Get getArg)
         {
            map[key] = new PropTemplate<Targ, T>(getArg);
         }

         template<typename T> void add(const std::string& key, typename SetterTemplate<Targ, T>::Set setArg)
         {
            map[key] = new PropTemplate<Targ, T>(setArg);
         }

         template<typename T> void add(const std::string& key, 
               typename GetterTemplate<Targ, T>::Get getArg, typename SetterTemplate<Targ, T>::Set setArg)
         {
            map[key] = new PropTemplate<Targ, T>(getArg, setArg);
         }

         std::map<std::string, const PropTemplateBase<Targ>*> map;
   };

   template<typename Targ> class HasProperties
   {
      public:
         virtual ~HasProperties() = default;

         template<typename T>
         static void addProperty(const std::string& key, typename PropTemplate<Targ, T>::Get getArg)
         {
            props().add(key, getArg);
         }

         template<typename T>
         static void addProperty(const std::string& key, typename PropTemplate<Targ, T>::Set setArg)
         {
            props().add(key, setArg);
         }

         template<typename T>
         static void addProperty(const std::string& key, 
               typename PropTemplate<Targ, T>::Get getArg, typename PropTemplate<Targ, T>::Set setArg)
         {
            props().add(key, getArg, setArg);
         }

         std::unique_ptr<PropBase<Targ>> property(const std::string& key)
         {
            auto it = props().map.find(key);
            return (it != props().map.end()) ? it->second->baseBind(targ()) : nullptr;
         }

         template<typename T> std::unique_ptr<Property<Targ, T>> property(const std::string& key)
         {
            auto it = props().map.find(key);
            return (it != props().map.end())
               ? (dynamic_cast<const PropTemplate<Targ, T>&>(*it->second)).bind(targ())
               : nullptr;
         }
         
         template<typename T> std::unique_ptr<const Property<Targ, T>> property(const std::string& key) const
         {
            auto it = props().map.find(key);
            return (it != props().map.end())
               ? (dynamic_cast<const PropTemplate<Targ, T>&>(*it->second)).bind(constTarg())
               : nullptr;
         }

         template<typename PropType = PropBase<Targ>>
         std::map<std::string, std::unique_ptr<PropType>> properties()
         {
            for (auto it : props().map)
            {
               auto prop = it->second->bind(targ());
               if (prop != nullptr)
               {
                  props().map[it->first] = std::move(prop);
               }
            }
         }

      private:
         static Properties<Targ>& props()
         {
            static Properties<Targ> sProps;
            return sProps;
         }

         const Targ* constTarg() const
         {
            return dynamic_cast<const Targ*>(this);
         }

         Targ* targ()
         {
            return dynamic_cast<Targ*>(this);
         }
   };
}

#endif // PROPERTIES_DOT_H
