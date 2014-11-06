#ifndef PROPERTY_DOTH
#define PROPERTY_DOTH

#include<map>
#include<stdexcept>
#include<string>
#include<vector>

namespace SmartGridToolbox
{
   class PropertyBase
   {

      public:

         virtual ~PropertyBase() = default;
   };

   template<class T> using ByValue = T;
   template<class T> using ByConstRef = const T&;

   template<typename T, template<typename> class RetByType> class Gettable
   {
      public:

         virtual RetByType<T> get() const = 0;
   };

   struct PropertyNone {};
   typedef PropertyNone NoTarget;
   typedef PropertyNone NoGetter;

   template<typename T, template<typename> class RetByType, typename TargType = NoTarget> class Property;

   template<typename T, template<typename> class RetByType>
      class Property<T, RetByType, NoGetter> : public PropertyBase, public Gettable<T, RetByType>
   {
      public:

         template<typename... Args> Property(Args&&... args) : val_(std::forward<Args>(args)...)
         {
            // Empty.
         }

         virtual RetByType<T> get() const override {return val_;}

      private:

         T val_;
   };

   template<typename T, template<typename> class RetByType, typename TargType>
      class Property : public PropertyBase, public Gettable<T, RetByType>
   {
      public:

         template<typename Arg> Property(const TargType& targ, Arg&& arg) : targ_(targ), get_(std::forward<Arg>(arg))
         {
            // Empty.
         }

         virtual RetByType<T> get() const override {return get_(targ_);}

      private:

         const TargType& targ_;
         std::function<RetByType<T> (const TargType&)> get_;
   };
}

#endif // PROPERTY_DOTH
