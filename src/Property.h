#ifndef PROPERTY_DOT_H
#define PROPERTY_DOT_H

// TODO: We need to sort out the all issues revolving around reference 
// collapsing, const references etc. in these templates.

#include<functional>
#include<iostream>

namespace SmartGridToolbox
{
   enum class PropType
   {
      GET,
      SET,
      BOTH
   };

   class PropertyBase
   {
      public:
         virtual ~PropertyBase()
         {
            // Empty.
         }
   };

   template <typename T, PropType type> class Property;

   template <typename T> class Property<T, PropType::GET> : 
      public virtual PropertyBase
   {
      public:
         template<typename G> Property(G get) : getFunction_(get)
         {
            // Empty.
         }

         T get() const
         {
            return getFunction_();
         }

      private:
         std::function<T ()> getFunction_;
   };

   template <typename T> class Property<T, PropType::SET> : 
      public virtual PropertyBase
   {
      public:
         template<typename S> Property(S set) : 
            setFunction_(set)
         {
            // Empty.
         }

         void set(const T & rhs) const
         {
            return setFunction_(rhs);
         }

      private:
         std::function<void(const T &)> setFunction_;
   };

   template <typename T> class Property<T, PropType::BOTH> : 
      public Property<T, PropType::GET>,
      public Property<T, PropType::SET>
   {
      public:
         template<typename G, typename S> Property(G get, S set) : 
            Property<T, PropType::GET>(get),
            Property<T, PropType::SET>(set)
         {
            // Empty.
         }
   };
}

#endif
