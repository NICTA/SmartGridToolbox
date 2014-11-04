#ifndef COMPONENT_DOT_H
#define COMPONENT_DOT_H

#include <SgtCore/Property.h>

#include <ostream>
#include <string>

using std::string;

namespace SmartGridToolbox
{
   class ComponentInterface
   {
      friend std::ostream& operator<<(std::ostream& os, const ComponentInterface& comp);

      public:
         virtual ~ComponentInterface() {}
         virtual const std::string& id() const = 0;
         virtual const char* componentType() const = 0;
         virtual const Properties & properties() const = 0;
         virtual Properties & properties() = 0;
      protected:
         virtual void print(std::ostream& os) const = 0;
   };

   inline std::ostream& operator<<(std::ostream& os, const ComponentInterface& comp)
   {
      comp.print(os);
      return os;
   }

   class Component : virtual public ComponentInterface
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         Component(const std::string& id) : id_(id) {}
         virtual ~Component() = default;

      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentType() const override {return "component";}

      /// @}
         
      /// @name Id:
      /// @{
         
         virtual const std::string& id() const override {return id_;}
         void setId(const std::string& id) {id_ = id;}

      /// @}
         
      /// @name Properties:
      /// @{
         
         virtual const Properties& properties() const
         {
            return properties_;
         }
         
         virtual Properties& properties()
         {
            return properties_;
         }

      /// @}
         
      protected:

      /// @name Printing:
      /// @{
         
         virtual void print(std::ostream& os) const
         {
            os << componentType() << ": " << id() << ":" << std::endl;
         }

      /// @}

      private:

         std::string id_ = "UNDEFINED";
         Properties properties_;
   };
}

#endif // COMPONENT_DOT_H
