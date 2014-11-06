#ifndef PROPERTY_DOT_H
#define PROPERTY_DOT_H

#include <SgtCore/Common.h>

#include<map>
#include<stdexcept>
#include<string>
#include<vector>

namespace SmartGridToolbox
{
   struct PropertyNone {};
   typedef PropertyNone NoTarget;
   typedef PropertyNone NoGetter;
   typedef PropertyNone NoSetter;

   class PropertyBase
   {

      public:

         virtual ~PropertyBase() = default;
   };

   template<typename TargetType_, typename GetterReturnType_> class Gettable
   {
      public:

         virtual GetterReturnType_ get(const TargetType_& target) const = 0;
   };

   template<typename TargetType_, typename SetterArgType_> class Settable
   {
      public:

         virtual void set(TargetType_& target, SetterArgType_ val) = 0;
   };

   template<typename BaseType_, typename TargetType_ = NoTarget, typename GetterReturnType_ = NoGetter, 
      typename SetterArgType_ = NoSetter> class Property;

   template<typename BaseType_>
      class Property<BaseType_, NoTarget, NoGetter, NoSetter> :
      public PropertyBase,
      public Gettable<NoTarget, const BaseType_&>,
      public Settable<NoTarget, const BaseType_&>
   {
      public:

         template<typename... Args> Property(Args&&... args) : val_(std::forward<Args>(args)...)
         {
            // Empty.
         }

         virtual const BaseType_& get() const override {return val_;}

         virtual void set(const BaseType_& val) override {val_ = val;}

      private:

         BaseType_ val_;
   };

   template<typename BaseType_, typename TargetType_, typename GetterReturnType_>
      class Property<BaseType_, TargetType_, GetterReturnType_, NoSetter> :
      public PropertyBase, public Gettable<TargetType_, const BaseType_&>
   {
      public:

         template<typename Arg> Property(Arg&& arg) : get_(std::forward<Arg>(arg))
         {
            // Empty.
         }

         virtual GetterReturnType_ get(const TargetType_& target) const {return get_(target);}

      private:

         std::function<GetterReturnType_ (const TargetType_&)> get_;
   };

   template<typename BaseType_, typename TargetType_, typename GetterReturnType_, typename SetterArgType_>
      class Property :
      public Property<BaseType_, TargetType_, GetterReturnType_>, public Settable<TargetType_, SetterArgType_>
   {
      public:

         template<typename GetterArg, typename SetterArg> Property(GetterArg&& getArg, SetterArg&& setArg) :
            Property<BaseType_, TargetType_, GetterReturnType_>(std::forward<GetterArg>(getArg)),
            set_(std::forward<SetterArg>(setArg))
         {
            // Empty.
         }
         
         virtual void set(TargetType_& target, SetterArgType_ val)
         {
            set_(target, val);
         }

      private:

         std::function<void (TargetType_&, SetterArgType_)> set_;
   };

   class HasProperties
   {
      private:

         typedef std::map<std::string, PropertyBase*> MapType;

      public:

         typedef MapType::const_iterator ConstIteratorType;
         typedef MapType::iterator IteratorType;

      public:

         ~HasProperties()
         {
            for (auto pair : properties_)
            {
               delete pair.second;
            }
         }

         template<typename BaseType_, typename TargetType_ = NoTarget, typename GetterReturnType_ = NoGetter,
            typename SetterArgType_ = NoSetter, typename... Args> void add(const std::string& key, Args&&... args)
         {
            properties_[key] = new Property<BaseType_, TargetType_, GetterReturnType_, SetterArgType_>(
                  std::forward<Args>(args)...);
         }

         template<typename TargetType_, typename GetterReturnType_>
            GetterReturnType_ get(const std::string& key) const
         {
            auto p = dynamic_cast<const Gettable<TargetType_, GetterReturnType_>*>(properties_.at(key));
            if (p == nullptr)
            {
               throw std::out_of_range(std::string("Property ") + key + " not found.");
            }
            return p->get(dynamic_cast<const TargetType_&>(*this));
         }
         
         template<typename TargetType_, typename SetterArgType_>
            void set(const std::string& key, const SetterArgType_& val)
         {
            auto p = dynamic_cast<const Settable<TargetType_, SetterArgType_>*>(properties_.at(key));
            if (p == nullptr)
            {
               throw std::out_of_range(std::string("Property ") + key + " not found.");
            }
            p->set(dynamic_cast<const TargetType_&>(*this), val);
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
