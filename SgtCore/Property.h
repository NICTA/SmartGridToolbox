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
   
   template<typename T, template<typename> class PassBy, class Targ>
   using Getter = PassBy<T> (const Targ&);

   class Properties;

   class PropertyAbc
   {
      friend class Properties;

      protected:
         virtual ~PropertyAbc() {};
         virtual void retarget(Properties* targ) = 0;
   };

   template<typename Targ> class HasTarget : virtual public PropertyAbc
   {
      protected:
         HasTarget() = default;

         HasTarget(Properties* targ) : targ_(dynamic_cast<Targ*>(targ)) {}

         virtual void retarget(Properties* targ) override
         {
            targ_ = dynamic_cast<Targ*>(targ);
         }

      protected:
         Targ* targ_;
   };

   template<typename T, template<typename> class PassBy>
   class GettableAbc
   {
      public:
         virtual PassBy<T> get() const = 0;
   };

   template<typename T, template<typename> class PassBy, typename Targ>
   class Gettable : public GettableAbc<T, PassBy>, virtual public HasTarget<Targ>
   {
      protected:
         Gettable(Getter<T, PassBy, Targ> arg) : get_(arg)
         {
            // Empty.
         }

      public:
         virtual PassBy<T> get() const override
         {
            return get_(*this->targ_);
         }
      
      private:
         std::function<Getter<T, PassBy, Targ>> get_;
   };

   // Full template declaration for a Property.
   template<typename T, template<typename> class PassBy, typename Targ> class Property;

   template<typename T, template<typename> class PassBy, typename Targ>
   class Property : public Gettable<T, PassBy, Targ>
   {
      public:

         template<typename GetterArg> Property(Properties* targ, GetterArg getterArg) :
            HasTarget<Targ>(targ),
            Gettable<T, PassBy, Targ>(getterArg)
         {
            // Empty.
         }
   };
   
   class Properties
   {
      private:
         typedef std::map<std::string, PropertyAbc*> Map;

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

         template<typename T, template<typename> class PassBy, typename Targ, typename Arg>
         void addProperty(const std::string& key, Arg arg)
         {
            map_[key] = new Property<T, PassBy, Targ>(this, arg);
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
