#ifndef COMPONENT_DOT_H
#define COMPONENT_DOT_H

#include <SgtCore/Properties.h>

#include <ostream>
#include <string>

using std::string;

namespace Sgt
{
   class ComponentInterface : virtual public HasPropertiesInterface
   {
      public:

      /// @name Lifecycle:
      /// @{

         virtual ~ComponentInterface() = default;

      /// @}

      /// @name Pure virtual fuctions (to be overridden):
      /// @{

         virtual const std::string& id() const = 0;

         virtual const std::string& componentType() const = 0;

         virtual void print(std::ostream& os) const = 0;

      /// @}
   };

   inline std::ostream& operator<<(std::ostream& os, const ComponentInterface& comp)
   {
      comp.print(os);
      return os;
   }

   class Component : virtual public ComponentInterface
   {
      public:

         SGT_PROPS_INIT(Component);

      /// @name Static member functions:
      /// @{

         static const std::string& sComponentType()
         {
            static std::string result("component");
            return result;
         }

      /// @}

      /// @name Lifecycle:
      /// @{

         Component(const std::string& id) :
            id_(id)
         {
            // Empty.
         }

         SGT_PROP_GET(id, Component, std::string, GetByConstRef, id);

      /// @}

      /// @name ComponentInterface virtual overridden functions.
      /// @{

         virtual const std::string& id() const
         {
            return id_;
         }

         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }

         SGT_PROP_GET(componentType, Component, std::string, GetByConstRef, componentType);

         virtual void print(std::ostream& os) const override
         {
            os << componentType() << ": " << id() << ":" << std::endl;
         }

      /// @}

      private:
         std::string id_;
   };
}

#endif // COMPONENT_DOT_H
