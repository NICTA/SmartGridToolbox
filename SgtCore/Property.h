#ifndef PROPERTY_DOT_H
#define PROPERTY_DOT_H

#include<map>
#include<vector>
#include<string>

namespace SmartGridToolbox
{
   class PropertyBase
   {
      public:
         virtual ~PropertyBase() = default;
   };

   template<typename ReturnType> class Gettable : virtual public PropertyBase
   {
      public:
         virtual ReturnType get() const = 0;
   };

   template<typename ArgType> class Settable : virtual public PropertyBase
   {
      public:
         virtual void set(ArgType val) = 0;
   };

   struct NoGetter {};
   struct NoSetter {};

   template<typename T, typename GetterReturnType = NoGetter, typename SetterArgType = NoSetter> class Property;

   template<typename T> class Property<T, NoGetter, NoSetter> :
      virtual public Gettable<const T&>, virtual public Settable<const T&>
   {
      public:

         template<typename... Args> Property(Args&&... args) : val_(std::forward<Args>(args)...)
         {
            // Empty.
         }

         virtual const T& get() const override {return val_;}

         virtual void set(const T& val) override {val_ = val;}

      private:

         T val_;
   };

   template<typename T, typename GetterReturnType> class Property<T, GetterReturnType, NoSetter> :
      virtual public Gettable<GetterReturnType>
   {
      public:

         template<typename Arg> Property(Arg&& arg) : get_(std::forward<Arg>(arg))
         {
            // Empty.
         }

         virtual GetterReturnType get() const {return get_();}

      private:

         std::function<GetterReturnType ()> get_;
   };

   template<typename T, typename GetterReturnType, typename SetterArgType> class Property :
      virtual public Property<T, GetterReturnType>, virtual public Settable<SetterArgType>
   {
      public:

         template<typename GetterArg, typename SetterArg> Property(GetterArg&& getArg, SetterArg&& setArg) :
            Property<T, GetterReturnType>(std::forward<GetterArg>(getArg)),
            set_(std::forward<SetterArg>(setArg))
         {
            // Empty.
         }
         
         void set(SetterArgType val)
         {
            set_(val);
         }

      private:

         std::function<void (SetterArgType)> set_;
   };

   class Properties
   {
      public:

         typedef std::map<std::string, PropertyBase*> MapType;
         typedef MapType::const_iterator ConstIteratorType;
         typedef MapType::iterator IteratorType;

         ~Properties()
         {
            for (auto pair : properties_)
            {
               delete pair.second;
            }
         }

         template<typename T, typename G = NoGetter, typename S = NoSetter, typename... Args>
            void add(const std::string& key, Args&&... args)
         {
            properties_[key] = new Property<T, G, S>(std::forward<Args>(args)...);
         }

         template<typename T> T get(const std::string& key) const
         {
            return dynamic_cast<const Gettable<T>*>(properties_.at(key))->get();
         }
         
         template<typename T> void set(const std::string& key, const T& val)
         {
            dynamic_cast<const Settable<T>*>(properties_.at(key))->set(val);
         }

         ConstIteratorType cbegin() const
         {
            return properties_.cbegin();
         }

         ConstIteratorType cend() const
         {
            return properties_.cend();
         }

         IteratorType begin()
         {
            return properties_.begin();
         }

         IteratorType end()
         {
            return properties_.end();
         }

      private:

         std::map<std::string, PropertyBase*> properties_;
   };
}

#endif // PROPERTY_DOT_H
