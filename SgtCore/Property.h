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

   template<typename T> class GettableProperty : virtual public PropertyBase
   {
      public:
         template<typename U = T> U get() const;
   };

   template<typename T> class SettableProperty : virtual public PropertyBase
   {
      public:
         virtual void set(const T& val) = 0;
   };

   struct NoGetter {};
   struct NoSetter {};

   template<typename T, typename G = NoGetter, typename S = NoSetter> class Property;

   template<typename T> class Property<T, NoGetter, NoSetter> :
      virtual public GettableProperty<T>, virtual public SettableProperty<T>
   {
      public:

         template<typename... Args> Property(Args&&... args) : val_(std::forward<Args>(args)...)
         {
            // Empty.
         }

         template<typename U = T> U get() const {return val_;}

         void set(const T& val) {val_ = val;}

      private:

         T val_;
   };

   template<typename T, typename G> class Property<T, G, NoSetter> : virtual public GettableProperty<T>
   {
      public:

         template<typename Arg> Property(Arg&& arg) : get_(std::forward<Arg>(arg))
         {
            // Empty.
         }

         template<typename U = T> U get() const {return get_();}

      private:

         std::function<G> get_;
   };

   template<typename T, typename G, typename S> class Property :
      virtual public Property<T, G>, virtual public SettableProperty<T>
   {
      public:

         template<typename GetterArg, typename SetterArg> Property(GetterArg&& getArg, SetterArg&& setArg) :
            Property<T, G>(std::forward<GetterArg>(getArg)),
            set_(std::forward<SetterArg>(setArg))
         {
            // Empty.
         }
         
         void set(const T& val)
         {
            set_(val);
         }

      private:

         std::function<S> set_;
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
            return dynamic_cast<const GettableProperty<T>*>(properties_.at(key))->get();
         }
         
         template<typename T> void set(const std::string& key, const T& val)
         {
            dynamic_cast<const SettableProperty<T>*>(properties_.at(key))->set(val);
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
