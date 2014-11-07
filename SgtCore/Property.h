#ifndef PROPERTY_DOTH
#define PROPERTY_DOTH

#include<map>
#include<stdexcept>
#include<string>
#include<vector>

namespace SmartGridToolbox
{
   struct NoTarget {};
   
   template<class T> using ByValue = T;
   template<class T> using ByConstRef = const T&;
   
   template<typename T, template<typename> class RetByType> class Gettable
   {
      public:
         virtual RetByType<T> get() const = 0;
      private:
   };

   // A property that is bound to a target. Designed to be created on the fly.
   class BoundPropertyBase
   {
      public:
         virtual ~BoundPropertyBase() {};
   };

   // Full template declaration for a BoundProperty.
   template<typename T, template<typename> class RetByType, typename TargType = NoTarget> class BoundProperty;

   // Stored value bound property (no target).
   template<typename T, template<typename> class RetByType>
      class BoundProperty<T, RetByType, NoTarget> : public Gettable<T, RetByType>
   {
      public:

         BoundProperty(T& val) : val_(val)
         {
            // Empty.
         }

         virtual RetByType<T> get() const override {return val_;}

      private:

         T& val_;
   };

   // Bound property taking a std::function of the bound target.
   template<typename T, template<typename> class RetByType, typename TargType>
      class BoundProperty : public BoundPropertyBase, public Gettable<T, RetByType>
   {
      public:

         virtual RetByType<T> get() const override {return get_(targ_);}
      
      private:

         template<typename Arg>
            BoundProperty(const TargType& targ, std::function<RetByType<T>>& get) : targ_(targ), get_(get)
         {
            // Empty.
         }

      private:

         const TargType& targ_;
         std::function<RetByType<T>& (const TargType&)> get_;
   };

   // Property that is not yet bound to a target.
   class UnboundPropertyBase
   {
      public:
         virtual ~UnboundPropertyBase() {};
   };

   template<typename TargType = NoTarget> class UnboundPropertyOfTargTypeBase : public UnboundPropertyBase
   {
      friend std::unique_ptr<BoundPropertyBase> bind(UnboundPropertyBase& unboundProp, TargType& targ);

      private:
         virtual std::unique_ptr<BoundPropertyBase> bind(TargType& targ) = 0; // Factory method.
   };

   template<typename TargType>
      std::unique_ptr<BoundPropertyBase> bind(UnboundPropertyBase& unboundProp, TargType& targ)
   {
      return (dynamic_cast<UnboundPropertyOfTargTypeBase<TargType>>(unboundProp))->bind(targ);
   }

   template<typename T, template<typename> class RetByType, typename TargType = NoTarget> class UnboundProperty;

   template<typename T, template<typename> class RetByType> class UnboundProperty<T, RetByType, NoTarget>
   {
      public:

         template<typename... Args> UnboundProperty(Args&&... args) : val_(std::forward<Args>(args)...)
         {
            // Empty.
         }

      private:

         virtual std::unique_ptr<BoundPropertyBase> bind(TargType& targ)
         {
            return std::unique_ptr<BoundProperty<T, 
         }

      private:

         T val_;
   };

}

#endif // PROPERTY_DOTH
