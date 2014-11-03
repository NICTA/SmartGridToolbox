#ifndef PROPERTY_DOT_H
#define PROPERTY_DOT_H

#include<map>
#include<string>

namespace SmartGridToolbox
{
   class PropertyBase
   {
      public:
         virtual ~PropertyBase() = default;
   };

   template<typename T> class Property : public PropertyBase
   {
      public:

         template<typename... Args> Property(Args&&... args) : val_(std::forward<Args>(args)...)
         {
            // Empty.
         }

         const T& val() const {return val_;}
         T& val() {return val_;}

         operator T() {return val_;}

      private:

         T val_;
   };

   template<typename T> class Property<T ()> : public PropertyBase
   {
      public:

         template<typename... Args> Property(Args&&... args) : func_(std::forward<Args>(args)...)
         {
            // Empty.
         }

         const T val() const {return func_();}
         T val() {return func_();}

         operator T() {return func_();}

      private:

         std::function<T ()> func_;
   };

   class Properties
   {
      public:

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

      private:

         std::map<std::string, PropertyBase*> properties_;
   };
}

#endif // PROPERTY_DOT_H
