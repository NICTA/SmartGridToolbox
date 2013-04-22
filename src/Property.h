#ifndef PROPERTY_DOT_H
#define PROPERTY_DOT_H

#include<functional>

class PropertyBase
{
   // Empty.
};
 
template <typename T> class Property : public PropertyBase
{
   public:
      Property(const std::function<const T & ()> & valueFunction) : 
         valueFunction_(valueFunction)
      {}

      const T & operator()() const
      {
         return valueFunction_();
      }

   private:
      const std::function<const T & ()> valueFunction_;
};

template <typename T> class Control : public Property<T>
{
   public:
      Control(const std::function<const T & ()> & valueFunction,
                        const std::function<void(const T &)> & setFunction) : 
         Property<T>(valueFunction),
         setFunction_(setFunction)
      {}

      void operator=(const T & rhs)
      {
         return setFunction_();
      }

   private:
      const std::function<const T & ()> valueFunction_;
      const std::function<const T & ()> setFunction_;
};

#endif
