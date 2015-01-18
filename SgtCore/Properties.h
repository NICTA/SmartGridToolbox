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

   template<typename Targ, typename T, bool hasGetter, bool hasSetter> class Property;

   template<typename Targ, typename T, bool hasGetter, bool hasSetter> class PropTemplate;

   class PropTemplateBase
   {
      // Empty.
   };

   template<typename Targ, typename T> class PropTemplate<Targ, T, true, false> : virtual public PropTemplateBase
   {
      friend class Property<Targ, T, true, false>;

      protected:
         PropTemplate(Get<Targ, T> getArg) : get_(getArg)
         {
            // Empty.
         }

         virtual ~PropTemplate() = default;

         std::function<Get<Targ, T>> get_;
   };
   template<typename Targ, typename T> using GetPropTemplate = PropTemplate<Targ, T, true, false>;
   
   template<typename Targ, typename T> class PropTemplate<Targ, T, false, true> : virtual public PropTemplateBase 
   {
      friend class Property<Targ, T, false, true>;

      protected:
         PropTemplate(Set<Targ, T> setArg) : set_(setArg)
         {
            // Empty.
         }

         virtual ~PropTemplate() = default;

         std::function<Set<Targ, T>> set_;
   };
   template<typename Targ, typename T> using SetPropTemplate = PropTemplate<Targ, T, false, true>;
   
   template<typename Targ, typename T> class PropTemplate<Targ, T, true, true> : 
      virtual public GetPropTemplate<Targ, T>, virtual public SetPropTemplate<Targ, T>
   {
      protected:
         PropTemplate(Get<Targ, T> getArg, Set<Targ, T> setArg) :
            GetPropTemplate<Targ, T>(getArg),
            SetPropTemplate<Targ, T>(setArg)
         {
            // Empty.
         }
   };
   template<typename Targ, typename T> using GetSetPropTemplate = PropTemplate<Targ, T, true, true>;

   template<typename Targ, typename T, bool isGettable, bool isSettable> class Property;

   template<typename Targ> class PropBase
   {
      public:
         PropBase(Targ* targ)
         {
            targ_(targ);
         }

         virtual ~PropBase() = default;

         virtual bool isGettable()
         {
            return false;
         }

         virtual bool isSettable()
         {
            return false;
         }

         virtual std::string string()
         {
            throw std::runtime_error("Property is not gettable");
         }

         virtual void setFromString(const std::string& str)
         {
            throw std::runtime_error("Property is not settable");
         }

         template<typename T, bool isGettable, bool isSettable>
         Property<Targ, T, isGettable, isSettable>* ofType()
         {
            return dynamic_cast<Property<Targ, T, isGettable, isSettable>>(this);
         }

      protected:
         Targ* targ_;
   };

   template<typename Targ, typename T> class GettableBase : virtual public PropBase<const Targ>
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

         virtual T get() = 0;
   };

   template<typename Targ, typename T> class SettableBase : virtual public PropBase<const Targ>
   {
      public:
         virtual bool isSettable() override
         {
            return true;
         }

         virtual std::string setFromString(const std::string& string) override
         {
            set(fromYamlString<T>(string));
         }

         virtual void set(const T& val) = 0;
   };

   template<typename Targ, typename T> class Property<Targ, T, true, false> : virtual public GettableBase<Targ, T>
   {
      public:
         Property(Targ* targ, PropTemplate<Targ, T, true, false>* propTemplate) : 
            targ_(targ), propTemplate_(propTemplate)
         {
            // Empty.
         }

         virtual T get()
         {
            return this->propTemplate_->get(*targ_);
         }

      private:
         Targ* targ_;
         PropTemplate<Targ, T, true, false>* propTemplate_;
   };

   template<typename Targ, typename T> class Property<Targ, T, false, true> :
      virtual public SettableBase<Targ, T>
   {
      public:
         Property(Targ* targ, PropTemplate<Targ, T, false, true>* propTemplate) : 
            targ_(targ), propTemplate_(propTemplate)
         {
            // Empty.
         }

         virtual void set(const T& val)
         {
            propTemplate_->set(*targ_, val);
         }

      private:
         Targ* targ_;
         PropTemplate<Targ, T, true, false>* propTemplate_;
   };

   template<typename Targ, typename T> class Property<Targ, T, true, true> :
      virtual public GettableBase<Targ, T>,
      virtual public SettableBase<Targ, T>
   {
      public:
         Property(Targ* targ, PropTemplate<Targ, T, true, true>* propTemplate) : 
            targ_(targ), propTemplate_(propTemplate)
         {
            // Empty.
         }

         virtual T get()
         {
            return this->propTemplate_->get(*targ_);
         }

         virtual void set(const T& val)
         {
            propTemplate_->set(*targ_, val);
         }

      private:
         Targ* targ_;
         PropTemplate<Targ, T, true, true>* propTemplate_;
   };

   template<typename Targ> class Properties
   {
      private:
         typedef std::map<std::string, PropTemplateBase*> Map;

      public:

         template<typename T> void add(const std::string& key, Get<Targ, T> getArg)
         {
            map_[key] = new PropTemplate<Targ, T, true, false>(getArg);
         }

         template<typename T> void add(const std::string& key, Set<Targ, T> setArg)
         {
            map_[key] = new PropTemplate<Targ, T, false, true>(setArg);
         }
         
         template<typename T> void add(const std::string& key, Get<Targ, T> getArg, Set<Targ, T> setArg)
         {
            map_[key] = new PropTemplate<Targ, T, true, true>(getArg, setArg);
         }

         template<typename T = NoType,
            template<typename> class GetBy = NoGetter, template<typename> class SetBy = NoSetter>
         const Property<T, GetBy, SetBy>* operator[](const std::string& key) const
         {
            const Property<T, GetBy, SetBy>* prop = nullptr;
            auto it = map_.find(key);
            if (it != map_.end())
            {
               prop = dynamic_cast<const Property<T, GetBy, SetBy>*>(it->second);
            }
            return prop;
         }

         template<typename T = NoType,
            template<typename> class GetBy = NoGetter, template<typename> class SetBy = NoSetter>
         Property<T, GetBy, SetBy>* operator[](const std::string& key)
         {
            return const_cast<Property<T, GetBy, SetBy>*>(
                  static_cast<const Properties*>(this)->operator[]<T, GetBy, SetBy>(key));
         }

      private:
         Properties(HasProperties* targ) : targ_(targ)
         {
            // Empty.
         }

         Properties(const Properties& from, HasProperties* targ) : map_(from.map_)
         {
            for (auto p : from.map_)
            {
               map_[p.first] = p.second->clone();
            }
            reTarget(targ);
         }

         virtual ~Properties()
         {
            for (auto& p : map_)
            {
               delete p.second;
            }
         }

         void reTarget(HasProperties* newTarg)
         {
            targ_ = newTarg;
            for (auto p : map_)
            {
               p.second->targ_ = newTarg;
            }
         }

      private:
         HasProperties* targ_;
         Map map_;
   };

   class HasPropertiesInterface
   {
      public:
         virtual ~HasPropertiesInterface()
         {
            // Empty.
         }
         virtual const Properties& properties() const = 0;
         virtual Properties& properties() = 0;
   };

   class HasProperties : virtual public HasPropertiesInterface
   {
      public:

         HasProperties() : properties_(this)
         {
            // Empty.
         }

         HasProperties(const HasProperties& from) : properties_(from.properties_, this)
         {
            // Empty.
         }

         virtual const Properties& properties() const override
         {
            return properties_;
         }

         virtual Properties& properties() override
         {
            return properties_;
         }

      private:

         Properties properties_;
   };
}

#endif // PROPERTIES_DOT_H
