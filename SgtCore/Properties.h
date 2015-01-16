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
   template<typename Targ, typename T, bool isGettable, bool isSettable> class Property;

   template<typename Targ> class PropertyBase
   {
      public:
         virtual ~PropertyBase()
         {
            // Empty.
         }

         virtual bool isGettable()
         {
            return false;
         }

         virtual bool isSettable()
         {
            return false;
         }

         virtual std::string string(const Targ& targ)
         {
            throw std::runtime_error("Property is not gettable");
         }

         virtual void setFromString(Targ& targ, const std::string& str)
         {
            throw std::runtime_error("Property is not settable");
         }

         template<typename T, bool isGettable, bool isSettable> Property<Targ, T, isGettable, isSettable>* ofType()
         {
            return dynamic_cast<Property<Targ, T, isGettable, isSettable>>(this);
         }
   };

   template<typename Targ, typename T> using Getter = T (const Targ&);
   template<typename Targ, typename T> using Setter = void (const Targ&, const T&);

   template<typename Targ, typename T> class Gettable : virtual public PropertyBase<Targ>
   {
      public:
         Gettable(Getter<Targ, T> getterArg) : get_(getterArg)
         {
            // Empty.
         }

         virtual ~Gettable()
         {
            // Empty.
         };

         virtual bool isGettable() override
         {
            return true;
         }

         virtual std::string string() override
         {
            return toYamlString(get());
         }

         T get(const Targ& targ)
         {
            return get_(targ);
         }

      private:
         std::function<Getter<Targ, T>> get_;
   };

   template<typename Targ, typename T> class Settable : virtual public PropertyBase<Targ>
   {
      public:
         Settable(Setter<Targ, T> setterArg) : set_(setterArg)
         {
            // Empty.
         }

         virtual ~Settable()
         {
            // Empty.
         };

         virtual bool isSettable() override
         {
            return true;
         }

         virtual void setFromString(const Targ& targ, const std::string& str) override
         {
            set(fromYamlString<T>(targ, str));
         }

         void set(Targ& targ, const T& val)
         {
            return set_(targ, val);
         }

      private:
         std::function<Setter<Targ, T>> set_;
   };

   template<typename Targ, typename T, bool isGettable, bool isSettable> class Property;

   template<typename Targ, typename T> class Property<Targ, T, true, false> : public Gettable<Targ, T>
   {
      public:
         Property(Getter<Targ, T> getterArg) : Gettable<Targ, T>(getterArg)
         {
            // Empty.
         }
   };

   template<typename Targ, typename T> class Property<Targ, T, false, true> : public Settable<Targ, T>
   {
      public:
         Property(Getter<Targ, T> getterArg) : Gettable<Targ, T>(getterArg)
         {
            // Empty.
         }

         virtual bool isSettable()
         {
            return false;
         }
   };

   template<typename Targ, typename T>
   class Property<Targ, T, true, true> : public Gettable<Targ, T>, public Settable<Targ, T>
   {
      public:
         Property(Getter<Targ, T> getterArg, Setter<Targ, T> setterArg) :
            Gettable<Targ, T>(getterArg),
            Settable<Targ, T>(setterArg)
         {
            // Empty.
         }
   };

   template<typename Targ> class Properties
   {
      private:
         typedef std::map<std::string, PropertyBase<Targ>*> Map;

      public:
         typedef typename Map::const_iterator ConstIterator;
         typedef typename Map::iterator Iterator;

      public:

         Iterator begin()
         {
            return map_.begin();
         }
         Iterator end()
         {
            return map_.end();
         }
         ConstIterator begin() const
         {
            return map_.begin();
         }
         ConstIterator end() const
         {
            return map_.end();
         }
         ConstIterator cbegin() const
         {
            return map_.cbegin();
         }
         ConstIterator cend() const
         {
            return map_.cend();
         }

         template<typename T, template<typename> class GetBy, template<typename> class SetBy,
            typename Targ, typename... Arg>
         void add(const std::string& key, Arg... args)
         {
            map_[key] = new PropertyWithTarg<T, GetBy, SetBy, Targ>(targ_, args...);
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
