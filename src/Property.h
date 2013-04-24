#ifndef PROPERTY_DOT_H
#define PROPERTY_DOT_H

// TODO: We need to sort out the all issues revolving around reference 
// collapsing, const references etc. in these templates.

#include<functional>
#include<iostream>

namespace SmartGridToolbox
{
   class PropertyBase
   {
      public:
         virtual ~PropertyBase()
         {
            // Empty.
         }
   };

   template <typename T> class ReadProperty : public virtual PropertyBase
   {
      public:
         template<typename G> ReadProperty(G get) : getFunction_(get)
         {
            // Empty.
         }

         T get() const
         {
            return getFunction_();
         }

         T operator()() const
         {
            return getFunction_();
         }

         operator T () const
         {
            return getFunction_();
         }

      private:
         std::function<T()> getFunction_;
   };

   template <typename T> class WriteProperty : public virtual PropertyBase
   {
      public:
         template<typename S> WriteProperty(S set) : 
            setFunction_(set)
         {
            // Empty.
         }

         void set(const T & rhs) const
         {
            return setFunction_(rhs);
         }

         T operator=(const T & rhs) const
         {
            return setFunction_(rhs);
         }

      private:
         std::function<void(const T &)> setFunction_;
   };

   template <typename T> class ReadWriteProperty : 
      public ReadProperty<T>, public WriteProperty<T>
   {
      public:
         template<typename G, typename S> ReadWriteProperty(G get, S set) : 
            ReadProperty<T>(get), WriteProperty<T>(set)
         {
            // Empty.
         }
   };
}

#endif
