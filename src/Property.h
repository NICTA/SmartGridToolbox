#ifndef PROPERTY_DOT_H
#define PROPERTY_DOT_H

#include<functional>

template <typename T> class Property
{
   public:
      Property(const std::function<const T & ()> & valueFunction) : 
         valueFunction_(valueFunction)
      {}

      const T & operator() const
      {
         return valueFunction_();
      }

   private:
      const std::function<const T & ()> valueFunction_;
};

template <typename T> class Control : public Property
{
   public:
      ReadWriteProperty(const std::function<const T & ()> & valueFunction,
                        const std::function<void(const T &)> & setFunction) : 
         ReadProperty(valueFunction),
         setFunction_(setFunction)
      {}

      const T & operator() const
      {
         return valueFunction_();
      }

      void operator=(const T & rhs)
      {
         return setFunction_();
      }

   private:
      const std::function<const T & ()> valueFunction_;
};

#endif
