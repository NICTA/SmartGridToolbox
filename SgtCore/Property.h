#ifndef PROPERTY_DOT_H
#define PROPERTY_DOT_H

#include<map>
#include<stdexcept>
#include<string>
#include<vector>

namespace SmartGridToolbox
{
   template<class T> using ByValue = T;
   template<class T> using ByConstRef = const T&;
   
   template<typename T, template<typename> class PassBy, class Targ> using Getter = PassBy<T> (const Targ&);
   template<typename T, template<typename> class PassBy, class Targ> using Setter = void (Targ&, PassBy<T>);

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

   template<typename T, template<typename> class PassBy> class GettableAbc : virtual public PropertyBase
   {
      friend class Properties;

      protected:
         virtual PassBy<T> get() const = 0;
   };

   template<typename T, template<typename> class PassBy, typename Targ> class Gettable : public GettableAbc<T, PassBy>
   {
      protected:
         Gettable(Getter<T, PassBy, Targ> arg) : get_(arg)
         {
            // Empty.
         }

      public:
         virtual PassBy<T> get() const override
         {
            return get_(*dynamic_cast<const Targ*>(this->targ_));
         }
      
      private:
         std::function<Getter<T, PassBy, Targ>> get_;
   };

   template<typename T, template<typename> class PassBy> class SettableAbc : virtual public PropertyBase
   {
      protected:
         virtual void set(PassBy<T>) = 0;
   };

   template<typename T, template<typename> class PassBy, typename Targ> class Settable : public SettableAbc<T, PassBy>
   {
      protected:
         Settable(Setter<T, PassBy, Targ> arg) : set_(arg)
         {
            // Empty.
         }

      public:
         virtual void set(PassBy<T> val) override
         {
            set_(*this->targ_, val);
         }
      
      private:
         std::function<Setter<T, PassBy, Targ>> set_;
   };

   enum class PropType
   {
      GET_ONLY,
      SET_ONLY,
      GET_AND_SET
   };

   // Full template declaration for a Property.
   template<typename T, PropType, template<typename> class PassBy, typename Targ> class Property;

   template<typename T, template<typename> class PassBy, typename Targ>
   class Property<T, PropType::GET_ONLY, PassBy, Targ> : public Gettable<T, PassBy, Targ>
   {
      public:

         template<typename GetterArg> Property(Properties* targ, GetterArg getterArg) :
            PropertyBase(targ),
            Gettable<T, PassBy, Targ>(getterArg)
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

         template<typename T, PropType PT, template<typename> class PassBy, typename Targ, typename Arg>
         void addProperty(const std::string& key, Arg arg)
         {
            map_[key] = new Property<T, PT, PassBy, Targ>(this, arg);
         }

         template<typename T, template<typename> class PassBy> PassBy<T> getProperty(const std::string& key)
         {
            GettableAbc<T, PassBy>* gettable;
            try
            {
               gettable = dynamic_cast<GettableAbc<T, PassBy>*>(map_.at(key));
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

      private:
         Map map_;
   };
}

#endif // PROPERTY_DOT_H
