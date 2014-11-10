#ifndef PROPERTY_DOTH
#define PROPERTY_DOTH

#include<map>
#include<stdexcept>
#include<string>
#include<vector>

namespace SmartGridToolbox
{
   struct NoTarget {};

   template<class T> using ByValue = T;
   template<class T> using ByConstRef = const T&;
   
   template<typename T, template<typename> class PassByType, class TargType>
   using Getter = PassByType<T> (const TargType&);
   
   template<typename T, template<typename> class PassByType, class TargType>
   using Setter = void (TargType&, PassByType<T>);

   // A property that is bound to a target. Designed to be created on the fly.
   class PropertyBase
   {
      public:
         virtual ~PropertyBase() {};
   };
   
   // Full template declaration for a Property.
   template<typename T, template<typename> class PassByType,
   bool hasGetter = false, bool hasSetter = false, typename TargType = NoTarget> class Property;

   // Stored value bound property (no target).
   template<typename T, template<typename> class PassByType>
   class Property<T, PassByType, false, false, NoTarget> : public PropertyBase
   {
      public:

         Property(PassByType<T> val) : val_(val)
         {
            // Empty.
         }

         PassByType<T> get() const {return val_;}
         
         void set(PassByType<T> val) {val_ = val;}

      private:

         T val_;
   };

   template<typename T, template<typename> class PassByType, typename TargType> class PropertyWithGetter
   {
      public:

         PropertyWithGetter(Getter<T, PassByType, TargType> arg) : get_(arg)
         {
            // Empty.
         }

         PassByType<T> get(const TargType& targ) const {return get_(targ);}
      
      private:

         std::function<Getter<T, PassByType, TargType>> get_;
   };

   template<typename T, template<typename> class PassByType, typename TargType> class PropertyWithSetter
   {

      public:

         PropertyWithSetter(Setter<T, PassByType, TargType> arg) : set_(arg)
         {
            // Empty.
         }

         void set(TargType& targ, PassByType<T> val) {return set_(targ, val);}
      
      private:

         std::function<Setter<T, PassByType, TargType>> set_;
   };

   template<typename T, template<typename> class PassByType, typename TargType>
   class Property<T, PassByType, true, false, TargType> :
      public PropertyWithGetter<T, PassByType, TargType>, public PropertyBase
   {
      public:
         template<typename GetterArg> Property(GetterArg getterArg) :
         PropertyWithGetter<T, PassByType, TargType>(getterArg)
         {
            // Empty.
         }
   };
   
   template<typename T, template<typename> class PassByType, typename TargType>
   class Property<T, PassByType, false, true, TargType> :
      public PropertyWithSetter<T, PassByType, TargType>, public PropertyBase
   {
      public:
         template<typename SetterArg> Property(SetterArg setterArg) :
         PropertyWithSetter<T, PassByType, TargType>(setterArg)
         {
            // Empty.
         }
   };
   
   template<typename T, template<typename> class PassByType, typename TargType>
   class Property<T, PassByType, true, true, TargType> :
      public PropertyWithGetter<T, PassByType, TargType>, public PropertyWithSetter<T, PassByType, TargType>,
      public PropertyBase
   {
      public:
         template<typename GetterArg, typename SetterArg>
         Property(GetterArg getterArg, SetterArg setterArg) :
            PropertyWithGetter<T, PassByType, TargType>(getterArg),
            PropertyWithSetter<T, PassByType, TargType>(setterArg)
         {
            // Empty.
         }
   };
   
   class Properties
   {
      template<typename T, template<typename> class PassByType, typename TargType>
      friend PassByType<T> getProperty(const TargType& targ, const std::string& key);
      
      template<typename T, template<typename> class PassByType, typename TargType>
      friend void setProperty(TargType& targ, const std::string& key, PassByType<T> val);
         
      template<typename T, template<typename> class PassByType, typename TargType>
      friend void addStoredProperty(TargType& targ, const std::string& key, PassByType<T> val);
      
      template<typename T, template<typename> class PassByType, typename TargType, typename GetterType>
      friend void addPropertyWithGetter(TargType& targ, const std::string& key, GetterType getter);
      
      template<typename T, template<typename> class PassByType, typename TargType, typename SetterType>
      friend void addPropertyWithSetter(TargType& targ, const std::string& key, SetterType setter);
      
      template<typename T, template<typename> class PassByType, typename TargType,
         typename GetterType, typename SetterType>
      friend void addPropertyWithGetterAndSetter(TargType& targ, const std::string& key,
            GetterType getter, SetterType setter);

      private:
         typedef std::map<std::string, PropertyBase*> MapType;

      public:
         typedef MapType::const_iterator ConstIterator;
         typedef MapType::iterator Iterator;
      
      public:
         ~Properties()
         {
            for (auto& pair : map_)
            {
               delete pair.second;
            }
         }

      private:
         MapType map_;
   };

   template<typename T, template<typename> class PassByType, typename TargType>
   PassByType<T> getProperty(const TargType& targ, const std::string& key)
   {
      auto propBase = targ.properties().map_.at(key);
      auto prop = dynamic_cast<const PropertyWithGetter<T, PassByType, TargType>*>(propBase);
      if (prop == nullptr)
      {
         auto propB = dynamic_cast<const Property<T, PassByType>*>(propBase);
         if (propB == nullptr)
         {
            throw std::out_of_range(std::string("No property with the key ") + key);
         }
         return propB->get();
      }
      return prop->get(targ);
   }

   template<typename T, template<typename> class PassByType, typename TargType>
   void setProperty(TargType& targ, const std::string& key, PassByType<T> val)
   {
      auto propBase = targ.properties().map_.at(key);
      auto prop = dynamic_cast<PropertyWithSetter<T, PassByType, TargType>*>(propBase);
      if (prop == nullptr)
      {
         auto propB = dynamic_cast<Property<T, PassByType>*>(propBase);
         if (propB == nullptr)
         {
            throw std::out_of_range(std::string("No property with the key ") + key);
         }
         propB->set(val);
      }
      prop->set(targ, val);
   }

   template<typename T, template<typename> class PassByType, typename TargType>
   void addStoredProperty(TargType& targ, const std::string& key, PassByType<T> val)
   {
      targ.properties().map_[key] = new Property<T, PassByType>(val);
   }
      
   template<typename T, template<typename> class PassByType, typename TargType, typename GetterType>
   void addPropertyWithGetter(TargType& targ, const std::string& key, GetterType getter)
   {
      targ.properties().map_[key] = new Property<T, PassByType, true, false, TargType>(getter);
   }
      
   template<typename T, template<typename> class PassByType, typename TargType, typename SetterType>
   void addPropertyWithSetter(TargType& targ, const std::string& key, SetterType setter)
   {
      targ.properties().map_[key] = new Property<T, PassByType, false, true, TargType>(setter);
   }
      
   template<typename T, template<typename> class PassByType, typename TargType,
      typename GetterType, typename SetterType>
   void addPropertyWithGetterAndSetter(TargType& targ, const std::string& key, GetterType getter, SetterType setter)
   {
      targ.properties().map_[key] = new Property<T, PassByType, true, true, TargType>(getter, setter);
   }
}

#endif // PROPERTY_DOTH
