#ifndef COMPONENT_DOT_H
#define COMPONENT_DOT_H

#include <string>

using std::string;

namespace SmartGridToolbox
{
   class ComponentAbc
   {
      public:
         virtual const std::string& id() const = 0;
         virtual const char* componentTypeStr() const = 0;
   };

   class Component : virtual public ComponentAbc
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         Component(const std::string& id) : id_(id) {}
         virtual ~Component() = default;

      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentTypeStr() const override {return "component";}

      /// @}
         
      /// @name Id:
      /// @{
         
         virtual const std::string& id() const override {return id_;}
         const void setId(const std::string& id) {id_ = id;}

      /// @}

      /// @name Stream output:
      /// @{
         
         friend std::ostream& operator<<(std::ostream& os, const Component& comp)
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
}

#endif // COMPONENT_DOT_H
