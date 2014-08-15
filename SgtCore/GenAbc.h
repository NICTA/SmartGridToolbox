#ifndef GEN_ABC_DOT_H
#define GEN_ABC_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/PowerFlow.h>

namespace SmartGridToolbox
{
   /// @brief A generation at a bus.
   class GenAbc : public Component
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         GenAbc(const std::string& id, Phases phases);

         virtual ~GenAbc() = default;
      
      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentTypeStr() const {return "gen";}

      /// @}

      /// @name Phase accessors:
      /// @{
         
         const Phases& phases() const
         {
            return phases_;
         }
         
         virtual void setPhases(const Phases& phases) 
         {
            phases_ = phases;
         }
        
      /// @}
 
      /// @name Status:
      /// @{
         
         virtual bool status() const = 0;

      /// @}
      
      /// @name Power injection:
      /// @{

         virtual const ublas::vector<Complex>& S() const = 0;

      /// @}
      
      /// @name Generation bounds:
      /// @{

         virtual double PMin() const = 0;
         virtual double PMax() const = 0;
         virtual double QMin() const = 0;
         virtual double QMax() const = 0;

      /// @}
      
      /// @name Generation costs:
      /// @{
          
         virtual double cStartup() const = 0;
         virtual double cShutdown() const = 0;
         virtual double c0() const = 0;
         virtual double c1() const = 0;
         virtual double c2() const = 0;
   
      /// @}
      
      protected:

         virtual void print(std::ostream& os) const override;
      
      private:

         Phases phases_;
   };
}

#endif // GEN_ABC_DOT_H
