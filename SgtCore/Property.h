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

   struct NoGetter {};
   struct NoSetter {};

   template<typename T, typename G = NoGetter, typename S = NoSetter> class Property;

   template<typename T> class Property<T, NoGetter, NoSetter> : public PropertyBase
   {
      public:

         template<typename... Args> Property(Args&&... args) : val_(std::forward<Args>(args)...)
         {
            // Empty.
         }

         const T& get() const {return val_;}

         void set(const T& val) const {val_ = val;}

      private:

         T val_;
   };

   template<typename T, typename G> class Property<T, G, NoSetter> : public PropertyBase
   {
      public:

         template<typename Arg> Property(Arg&& arg) : get_(std::forward<Arg>(arg))
         {
            // Empty.
         }

         T get() const {return get_();}

      private:

         std::function<G> get_;
   };

   template<typename T, typename G, typename S> class Property : public Property<T, G>
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

         ~Properties()
         {
            for (auto pair : properties_)
            {
               delete pair.second;
            }
         }

         template<typename T, typename... Args> void add(const std::string& key, Args&&... args)
         {
            properties_[key] = new Property<T>(std::forward<Args>(args)...);
         }

         template<typename T> Property<T>* get(const std::string& key)
         {
            auto it = properties_.find(key);
            return it == properties_.end() ? nullptr : dynamic_cast<Property<T>*>(it->second);
         }

         ConstIteratorType begin()
         {
            return properties_.cbegin();
         }

         ConstIteratorType end()
         {
            return properties_.cend();
         }

      private:

         std::map<std::string, PropertyBase*> properties_;
   };
}

#endif // PROPERTY_DOT_H
