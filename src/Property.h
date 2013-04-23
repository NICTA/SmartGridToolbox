#ifndef PROPERTY_DOT_H
#define PROPERTY_DOT_H

#include<functional>
#include<iostream>

class PropertyBase
{
   public:
      virtual ~PropertyBase()
      {
         // Empty.
      }
};
 
template <typename T> class Property : public PropertyBase
{
   public:
      template<typename U> Property(U func) : valueFunction_(func)
      {
         // Empty.
      }

      virtual T getValue() const
      {
         return valueFunction_();
      }

      virtual operator T () const
      {
         return valueFunction_();
      }

   private:
      std::function<T()> valueFunction_;
};

#endif
