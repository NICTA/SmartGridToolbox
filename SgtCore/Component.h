#ifndef COMPONENT_ABC_DOT_H
#define COMPONENT_ABC_DOT_H

#include <string>

using std::string;

namespace SmartGridToolbox
{
   class Component
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         Component(const std::string& id) : id_(id) {}
         virtual ~Component() = default;

      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentType() const {return "Component";}

      /// @}
         
      /// @name Id:
      /// @{
         
         const std::string& id() const {return id_;}
         const void setId(const std::string& id) {id_ = id;}

      /// @}

      private:

         std::string id_ = "UNDEFINED";
   };
}

#endif // COMPONENT_ABC_DOT_H
