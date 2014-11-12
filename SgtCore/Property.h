#ifndef PROPERTY_DOT_H
#define PROPERTY_DOT_H

#include<map>
#include<stdexcept>
#include<string>
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

   template<typename T, template<typename> class GetBy> class GettableAbc : virtual public PropertyBase
   {
      friend class Properties;

      protected:
         virtual GetBy<T> get() const = 0;
   };

   template<typename T, template<typename> class SetBy> class SettableAbc : virtual public PropertyBase
   {
      friend class Properties;

      private:
         virtual void set(SetBy<T>) = 0;
   };

   template<typename T, template<typename> class GetBy, typename Targ> class Gettable : public GettableAbc<T, GetBy>
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

   template<typename T, template<typename> class SetBy, typename Targ> class Settable : public SettableAbc<T, SetBy>
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

         template<typename T, template<typename> class GetBy> GetBy<T> getProperty(const std::string& key)
         {
            GettableAbc<T, GetBy>* gettable;
            try
            {
               gettable = dynamic_cast<GettableAbc<T, GetBy>*>(map_.at(key));
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

         template<typename T, template<typename> class SetBy> void setProperty(const std::string& key, SetBy<T> val)
         {
            SettableAbc<T, SetBy>* settable;
            try
            {
               settable = dynamic_cast<SettableAbc<T, SetBy>*>(map_.at(key));
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

      private:
         Map map_;
   };
}

#endif // PROPERTY_DOT_H
