#ifndef PROPERTIES_DOT_H
#define PROPERTIES_DOT_H

#include<map>
#include<stdexcept>
#include<string>
#include<sstream>
#include<vector>

namespace SmartGridToolbox
{
   struct PropertyNone {};

   template<class T> using ByValue = T;
   template<class T> using ByConstRef = const T&;
   template<class T> class NoGetter{};  
   template<class T> using NoSetter = NoGetter<T>;  
   
   template<typename T, template<typename> class GetBy, class Targ> using Getter = GetBy<T> (const Targ&);
   template<typename T, template<typename> class SetBy, class Targ> using Setter = void (Targ&, SetBy<T>);
   
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

   class PropertyBase
   {
      friend class Properties;

      protected:
         PropertyBase() = default;
         PropertyBase(Properties* targ) : targ_(targ) {}
         virtual ~PropertyBase() {}

      protected:
         Properties* targ_{nullptr};
   };

   class GettableBase : virtual public PropertyBase
   {
      friend class Properties;

      protected:
         virtual std::string getAsString() = 0;
   };

   class SettableBase : virtual public PropertyBase
   {
      friend class Properties;

      protected:
         virtual void setFromString(const std::string& str) = 0;
   };

   template<typename T, template<typename> class GetBy> class GettableTBase : public GettableBase
   {
      friend class Properties;

      protected:
         virtual GetBy<T> get() const = 0;

         virtual std::string getAsString()
         {
            using namespace std;
            return to_string(get());
         }

      private:
         std::function<std::string (GetBy<T>)> stringGetter_;
   };

   template<typename T, template<typename> class SetBy> class SettableTBase : virtual public SettableBase
   {
      friend class Properties;

      private:
         virtual void set(SetBy<T>) = 0;

         virtual void setFromString(const std::string& str)
         {
            set(FromString(str)); 
         }
   };

   template<typename T, template<typename> class GetBy, typename Targ> class Gettable : public GettableTBase<T, GetBy>
   {
      protected:
         Gettable(Getter<T, GetBy, Targ> arg) : get_(arg)
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

   template<typename T, template<typename> class SetBy, typename Targ> class Settable : public SettableTBase<T, SetBy>
   {
      protected:
         Settable(Setter<T, SetBy, Targ> arg) : set_(arg)
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

   // Full template declaration for a Property.
   template<typename T, template<typename> class GetBy, template<typename> class SetBy, typename Targ> class Property;

   template<typename T, template<typename> class GetBy, typename Targ>
   class Property<T, GetBy, NoSetter, Targ> : public Gettable<T, GetBy, Targ>
   {
      public:

         template<typename GetterArg> Property(Properties* targ, GetterArg getterArg) :
            PropertyBase(targ),
            Gettable<T, GetBy, Targ>(getterArg)
         {
            // Empty.
         }
   };

   template<typename T, template<typename> class SetBy, typename Targ>
   class Property<T, NoGetter, SetBy, Targ> : public Settable<T, SetBy, Targ>
   {
      public:

         template<typename SetterArg> Property(Properties* targ, SetterArg setterArg) :
            PropertyBase(targ),
            Settable<T, SetBy, Targ>(setterArg)
         {
            // Empty.
         }
   };

   template<typename T, template<typename> class GetBy, template<typename> class SetBy, typename Targ>
   class Property : public Gettable<T, GetBy, Targ>, public Settable<T, SetBy, Targ>
   {
      public:

         template<typename GetterArg, typename SetterArg>
         Property(Properties* targ, GetterArg getterArg, SetterArg setterArg) :
            PropertyBase(targ),
            Gettable<T, GetBy, Targ>(getterArg),
            Settable<T, SetBy, Targ>(setterArg)
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

         template<typename T, template<typename> class GetBy, template<typename> class SetBy, 
            typename Targ, typename... Arg>
         void addProperty(const std::string& key, Arg... args)
         {
            map_[key] = new Property<T, GetBy, SetBy, Targ>(this, args...);
         }

         template<typename T, template<typename> class GetBy> GetBy<T> property(const std::string& key)
         {
            GettableTBase<T, GetBy>* gettable;
            try
            {
               gettable = dynamic_cast<GettableTBase<T, GetBy>*>(map_.at(key));
            }
            catch (std::out_of_range e)
            {
               throw std::out_of_range(std::string("Property ") + key + " does not exist.");
            }
            if (gettable == nullptr)
            {
               throw std::out_of_range(std::string("Property ") + key + " exists, but is not gettable.");
            }
            return gettable->get();
         }

         std::string propertyAsString(const std::string& key)
         {
            return dynamic_cast<GettableBase*>(map_.at(key))->getAsString();
         }

         template<typename T, template<typename> class SetBy> void setProperty(const std::string& key, SetBy<T> val)
         {
            SettableTBase<T, SetBy>* settable;
            try
            {
               settable = dynamic_cast<SettableTBase<T, SetBy>*>(map_.at(key));
            }
            catch (std::out_of_range e)
            {
               throw std::out_of_range(std::string("Property ") + key + " does not exist.");
            }
            if (settable == nullptr)
            {
               throw std::out_of_range(std::string("Property ") + key + " exists, but is not settable.");
            }
            settable->set(val);
         }
         
         void setPropertyFromString(const std::string& key, const std::string& str)
         {
            dynamic_cast<SettableBase*>(map_.at(key))->setFromString(str);
         }

      private:
         Map map_;
   };
}

#endif // PROPERTIES_DOT_H
