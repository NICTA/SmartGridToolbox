#ifndef COMPONENT_DOT_H
#define COMPONENT_DOT_H

#include <string>

using std::string;

namespace SmartGridToolbox
{
   class ComponentVirtualBase
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         ComponentVirtualBase(const std::string& id) : id_(id) {}
         virtual ~ComponentVirtualBase() = default;

      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentTypeStr() const {return "component";}

      /// @}
         
      /// @name Id:
      /// @{
         
         const std::string& id() const {return id_;}
         const void setId(const std::string& id) {id_ = id;}

      /// @}

      /// @name Stream output:
      /// @{
         
         friend std::ostream& operator<<(std::ostream& os, const ComponentVirtualBase& comp)
         {
            comp.print(os);
            return os;
         }

      /// @}
      
      protected:

         virtual void print(std::ostream& os) const;

      private:

         std::string id_ = "UNDEFINED";
   };

   // Ensure that all Component base classes are, essentially, virtual.
   class Component : public ComponentVirtualBase
   {
      public:
         Component(const std::string& id): ComponentVirtualBase(id) {}
   };
}

#endif // COMPONENT_DOT_H
