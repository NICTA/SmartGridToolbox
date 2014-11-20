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
   struct NoType {};

   template<class T>
   class NoGetter{};  

   template<class T>
   using NoSetter = NoGetter<T>;  

   template<class T>
   using ByValue = T;

   template<class T>
   using ByConstRef = const T&;
   
   class Properties;
   class HasProperties;

   template<typename T = NoType,
      template<typename> class GetBy = NoGetter, template<typename> class SetBy = NoSetter>
   class Property;

   class PropertyBase
   {
      public:
         PropertyBase(HasProperties* targ) : targ_(targ) {}

         virtual ~PropertyBase() {}

         virtual bool isGettable() {return false;}

         virtual bool isSettable() {return false;}

         virtual std::string string()
         {
            throw std::runtime_error("Property is not gettable");
         }

         virtual void setFromString(const std::string& str)
         {
            throw std::runtime_error("Property is not settable");
         }

         const HasProperties* targ() const
         {
            return targ_;
         }

         HasProperties* targ()
         {
            return targ_;
         }

      protected:
         HasProperties* targ_{nullptr};
   };
  
   template<typename T, template<typename> class GetBy>
   class Property<T, GetBy, NoSetter> : virtual public PropertyBase
   {
      friend class Properties;

      public:
         virtual bool isGettable() {return true;}

         virtual GetBy<T> get() const = 0;

         virtual std::string string()
         {
            return toYamlString(get());
         }
   };

   template<typename T, template<typename> class GetBy>
   using GettableProperty = Property<T, GetBy, NoSetter>;

   template<typename T, template<typename> class SetBy>
   class Property<T, NoGetter, SetBy> : virtual public PropertyBase
   {
      friend class Properties;

      public:
         virtual bool isSettable() {return true;}

         virtual void set(SetBy<T>) = 0;

         virtual void setFromString(const std::string& str) override
         {
            set(fromYamlString<T>(str)); 
         }
   };
   
   template<typename T, template<typename> class SetBy>
   using SettableProperty = Property<T, NoGetter, SetBy>;

   template<typename T, template<typename> class GetBy, template<typename> class SetBy>
   class Property : virtual public GettableProperty<T, GetBy>, virtual public SettableProperty<T, SetBy>
   {
      // Empty.
   };
   
   template<typename T, template<typename> class GetBy, class Targ>
   using Getter = GetBy<T> (const Targ&);

   template<typename T, template<typename> class SetBy, class Targ>
   using Setter = void (Targ&, SetBy<T>);

   template<typename T, template<typename> class GetBy, template<typename> class SetBy, typename Targ>
   class PropertyWithTarg;

   template<typename T, template<typename> class GetBy, typename Targ>
   class PropertyWithTarg<T, GetBy, NoSetter, Targ> : virtual public GettableProperty<T, GetBy>
   {
      friend class Properties;

      public:
         PropertyWithTarg(HasProperties* targ, Getter<T, GetBy, Targ> getterArg) : PropertyBase(targ), get_(getterArg)
         {
            // Empty.
         }

         virtual GetBy<T> get() const override
         {
            return get_(*dynamic_cast<const Targ*>(this->targ_));
         }
      
      private:
         std::function<Getter<T, GetBy, Targ>> get_;
   };

   template<typename T, template<typename> class SetBy, typename Targ>
   class PropertyWithTarg<T, NoGetter, SetBy, Targ> : virtual public SettableProperty<T, SetBy>
   {
      friend class Properties;

      public:
         PropertyWithTarg(HasProperties* targ, Setter<T, SetBy, Targ> setterArg) : PropertyBase(targ), set_(setterArg)
         {
            // Empty.
         }

         virtual void set(SetBy<T> val) override
         {
            set_(*dynamic_cast<Targ*>(this->targ_), val);
         }
      
      private:
         std::function<Setter<T, SetBy, Targ>> set_;
   };

   template<typename T, template<typename> class GetBy, template<typename> class SetBy, typename Targ>
   class PropertyWithTarg :
      public PropertyWithTarg<T, GetBy, NoSetter, Targ>,
      public PropertyWithTarg<T, NoGetter, SetBy, Targ>,
      public Property<T, GetBy, SetBy>
   {
      friend class Properties;

      public:
         PropertyWithTarg(HasProperties* targ, Getter<T, GetBy, Targ> getterArg, Setter<T, SetBy, Targ> setterArg) :
            PropertyBase(targ),
            PropertyWithTarg<T, GetBy, NoSetter, Targ>(targ, getterArg),
            PropertyWithTarg<T, NoGetter, SetBy, Targ>(targ, setterArg)
         {
            // Empty.
         }
   };

   class Properties
   {
      friend class HasProperties;

      private:
         typedef std::map<std::string, PropertyBase*> Map;

      public:
         typedef Map::const_iterator ConstIterator;
         typedef Map::iterator Iterator;
      
      public:

         Iterator begin() {return map_.begin();}
         Iterator end() {return map_.end();}
         ConstIterator begin() const {return map_.begin();}
         ConstIterator end() const {return map_.end();}
         ConstIterator cbegin() const {return map_.cbegin();}
         ConstIterator cend() const {return map_.cend();}

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
         Properties(HasProperties* targ) : targ_(targ) {}
         
         Properties(const Properties& from, HasProperties* targ) : map_(from.map_)
         {
            reTarget(targ);
         }

         virtual ~Properties()
         {
            for (auto& pair : map_)
            {
               delete pair.second;
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
         virtual const Properties& properties() const = 0;
         virtual Properties& properties() = 0;
   };

   class HasProperties : virtual public HasPropertiesInterface
   {
      public:

         HasProperties() : properties_(this) {}

         HasProperties(const HasProperties& from) : properties_(from.properties_, this) {}

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
