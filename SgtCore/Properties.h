#ifndef PROPERTIES_DOT_H
#define PROPERTIES_DOT_H

#include<map>
#include<stdexcept>
#include<string>
#include<sstream>
#include<vector>

namespace SmartGridToolbox
{
   struct NoType {};

   template<class T> class NoGetter{};  
   template<class T> using NoSetter = NoGetter<T>;  

   template<class T> using ByValue = T;
   template<class T> using ByConstRef = const T&;
   
   
   class FromString{
      public:
         FromString(const std::string& str) : str_(str) {} 

         template<typename T> operator T()
         {
            T t;
            std::stringstream ss(str_);
            ss >> t;
            return t;
         }

      private:
         std::string str_;
   };

   class Properties;

   template<typename T = NoType,
      template<typename> class GetBy = NoGetter, template<typename> class SetBy = NoSetter>
   class Property;

   template<> class Property<NoType, NoGetter, NoSetter>
   {
      friend class Properties;

      public:
         virtual bool isGettable() {return false;}

         virtual bool isSettable() {return false;}

         virtual std::string getAsString()
         {
            throw std::runtime_error("Property is not gettable");
         }

         virtual void setFromString(const std::string& str)
         {
            throw std::runtime_error("Property is not settable");
         }

      protected:
         Property() = default;
         Property(Properties* targ) : targ_(targ) {}
         virtual ~Property() {}

      protected:
         Properties* targ_{nullptr};
   };
  
   using PropertyBase = Property<NoType, NoGetter, NoSetter>;

   template<typename T, template<typename> class GetBy>
   class Property<T, GetBy, NoSetter> : virtual public PropertyBase
   {
      friend class Properties;

      public:
         virtual bool isGettable() {return true;}

         virtual GetBy<T> get() const = 0;

         virtual std::string getAsString()
         {
            using namespace std;
            return to_string(get());
         }
   };

   template<typename T, template<typename> class GetBy> using GettableProperty = Property<T, GetBy, NoSetter>;

   template<typename T, template<typename> class SetBy>
   class Property<T, NoGetter, SetBy> : virtual public PropertyBase
   {
      friend class Properties;

      public:
         virtual bool isSettable() {return true;}

         virtual void set(SetBy<T>) = 0;

         virtual void setFromString(const std::string& str) override
         {
            set(FromString(str)); 
         }
   };
   
   template<typename T, template<typename> class SetBy> using SettableProperty = Property<T, NoGetter, SetBy>;

   template<typename T, template<typename> class GetBy, template<typename> class SetBy>
   class Property : virtual public GettableProperty<T, GetBy>, virtual public SettableProperty<T, SetBy>
   {
      // Empty.
   };
   
   template<typename T, template<typename> class GetBy, class Targ> using Getter = GetBy<T> (const Targ&);
   template<typename T, template<typename> class SetBy, class Targ> using Setter = void (Targ&, SetBy<T>);

   template<typename T, template<typename> class GetBy, template<typename> class SetBy, typename Targ>
   class PropertyWithTarg;

   template<typename T, template<typename> class GetBy, typename Targ>
   class PropertyWithTarg<T, GetBy, NoSetter, Targ> : virtual public GettableProperty<T, GetBy>
   {
      friend class Properties;

      protected:
         PropertyWithTarg(Properties* targ, Getter<T, GetBy, Targ> getterArg) : PropertyBase(targ), get_(getterArg)
         {
            // Empty.
         }

      public:
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

      protected:
         PropertyWithTarg(Properties* targ, Setter<T, SetBy, Targ> setterArg) : PropertyBase(targ), set_(setterArg)
         {
            // Empty.
         }

      public:
         virtual void set(SetBy<T> val) override
         {
            set_(*dynamic_cast<Targ*>(this->targ_), val);
         }
      
      private:
         std::function<Setter<T, SetBy, Targ>> set_;
   };

   template<typename T, template<typename> class GetBy, template<typename> class SetBy, typename Targ>
   class PropertyWithTarg :
      virtual public PropertyWithTarg<T, GetBy, NoSetter, Targ>,
      virtual public PropertyWithTarg<T, NoGetter, SetBy, Targ>,
      virtual public Property<T, GetBy, SetBy>
   {
      friend class Properties;

      protected:
         PropertyWithTarg(Properties* targ, Getter<T, GetBy, Targ> getterArg, Setter<T, SetBy, Targ> setterArg) :
            PropertyBase(targ),
            PropertyWithTarg<T, GetBy, NoSetter, Targ>(targ, getterArg),
            PropertyWithTarg<T, NoGetter, SetBy, Targ>(targ, setterArg)
         {
            // Empty.
         }
   };

   class Properties
   {
      private:
         typedef std::map<std::string, PropertyBase*> Map;

      public:
         typedef Map::const_iterator ConstIterator;
         typedef Map::iterator Iterator;
      
      public:
         Properties() = default;

         Properties(const Properties&& from) : map_(from.map_)
         {
            for (auto pair : map_)
            {
               pair.second->targ_ = this;
            }
         };

         Properties(const Properties& from) : map_(from.map_)
         {
            for (auto pair : map_)
            {
               pair.second->targ_ = this;
            }
         };

         virtual ~Properties()
         {
            for (auto& pair : map_)
            {
               delete pair.second;
            }
         }

         ConstIterator cbegin() const {return map_.cbegin();}
         ConstIterator cend() const {return map_.cend();}
         Iterator begin() {return map_.begin();}
         Iterator end() {return map_.end();}

         template<typename T, template<typename> class GetBy, template<typename> class SetBy, 
            typename Targ, typename... Arg>
         void addProperty(const std::string& key, Arg... args)
         {
            map_[key] = new PropertyWithTarg<T, GetBy, SetBy, Targ>(this, args...);
         }

         bool propertyIsGettable(const std::string& key)
         {
            return map_.at(key)->isGettable();
         }
         
         bool propertyIsSettable(const std::string& key)
         {
            return map_.at(key)->isSettable();
         }

         template<typename T = NoType,
            template<typename> class GetBy = NoGetter, template<typename> class SetBy = NoSetter>
         const Property<T, GetBy, SetBy>* property(const std::string& key) const
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
         Property<T, GetBy, SetBy>* property(const std::string& key)
         {
            return const_cast<Property<T, GetBy, SetBy>*>(
                  static_cast<const Properties*>(this)->property<T, GetBy, SetBy>(key));
         }

      private:
         Map map_;
   };
}

#endif // PROPERTIES_DOT_H
