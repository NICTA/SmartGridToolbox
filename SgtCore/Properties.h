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
   
   template<typename Targ, typename T> class PropTemplate;

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
         using Set = void (const Targ&, const T&);

         Setter(Set setArg) : set_(setArg)
         {
            // Empty.
         }

         void set(Targ* targ, const T& val)
         {
            set_(targ, val);
         }

      private:
         std::function<Set> set_;
   };
   
   template<typename Targ, typename T> class Property;

   template<typename Targ> class PropBase
   {
      public:
         template<typename T> T get() {return dynamic_cast<Property<Targ, T>>(this)->get();}
         template<typename T> void set(const T& val) {dynamic_cast<Property<Targ, T>>(this)->set(val);}
   };
   
   template<typename Targ, typename T> class Property : public PropBase<Targ>
   {
      public:
         Property(const PropTemplate<Targ, T>* propTemplate, Targ* targ) :
            propTemplate_(propTemplate), targ_(targ)
         {
            // Empty.
         }

         T get();

         void set(const T& val);

      private:
         const PropTemplate<Targ, T>* propTemplate_;
         Targ* targ_;
   };

   template<typename Targ> class HasProperties;

   template<typename Targ> class PropTemplateBase
   {
      public:
         virtual std::unique_ptr<PropBase<Targ>> baseBind(const Targ* targ)
         {
            return nullptr;
         }
   };

   template<typename Targ, typename T> class PropTemplate : public PropTemplateBase<Targ>
   {
      public:
         PropTemplate(typename Getter<Targ, T>::Get getArg) :
            getter_(new Getter<Targ, T>(getArg))
         {
            // Empty.
         }

         PropTemplate(typename Setter<Targ, T>::Set setArg) :
            setter_(new Setter<Targ, T>(setArg))
         {
            // Empty.
         }
         
         PropTemplate(typename Getter<Targ, T>::Get getArg, typename Setter<Targ, T>::Set setArg) :
            getter_(new Getter<Targ, T>(getArg)),
            setter_(new Setter<Targ, T>(setArg))
         {
            // Empty.
         }

         T get(const Targ& targ) const
         {
            return getter_->get(targ);
         }
         
         void set(Targ& targ, const T& val)
         {
            setter_->set(targ, val);
         }
         
         virtual std::unique_ptr<PropBase<Targ>> baseBind(Targ* targ) const
         {
            return std::unique_ptr<Property<Targ, T>>(new Property<Targ, T>(this, targ));
         }

         std::unique_ptr<Property<Targ, T>> bind(Targ* targ) const
         {
            return std::unique_ptr<Property<Targ, T>>(new Property<Targ, T>(this, targ));
         }

      private:
         std::unique_ptr<Getter<Targ, T>> getter_{nullptr};
         std::unique_ptr<Setter<Targ, T>> setter_{nullptr};
   };

   template<typename Targ, typename T> T Property<Targ, T>::get()
   {
      return propTemplate_->get(*targ_);
   }
   
   template<typename Targ, typename T> void Property<Targ, T>::set(const T& val)
   {
      propTemplate_->set(*targ_, val);
   }
   
   template<typename Targ> class Properties
   {
      public:
         Properties();

         template<typename T> void add(const std::string& key, typename Getter<Targ, T>::Get getArg)
         {
            map[key] = std::unique_ptr<const PropTemplate<Targ, T>>(new const PropTemplate<Targ, T>(getArg));
         }

         template<typename T> void add(const std::string& key, typename Setter<Targ, T>::Set setArg)
         {
            map[key] = std::unique_ptr<const PropTemplate<Targ, T>>(new const PropTemplate<Targ, T>(setArg));
         }

         template<typename T> void add(const std::string& key, 
               typename Getter<Targ, T>::Get getArg, typename Setter<Targ, T>::Set setArg)
         {
            map[key] = std::unique_ptr<const PropTemplate<Targ, T>>(new const PropTemplate<Targ, T>(getArg, setArg));
         }

         std::map<std::string, std::unique_ptr<const PropTemplateBase<Targ>>> map;
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

         Targ* targ()
         {
            return dynamic_cast<Targ*>(this);
         }
   };
}

#endif // PROPERTIES_DOT_H
