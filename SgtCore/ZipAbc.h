#ifndef ZIP_ABC_DOT_H
#define ZIP_ABC_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/PowerFlow.h>

#include<string>

namespace SmartGridToolbox
{
   /// @brief A Zip is an injection into a bus with constant impedance / current / complex power components.
   class ZipAbc : public Component
   {
      public:
      
      /// @name Lifecycle:
      /// @{

         ZipAbc(const std::string& id, Phases phases);

      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentTypeStr() const {return "zip_abc";}

      /// @}

      /// @name Basic identity and type:
      /// @{
 
         const Phases& phases() const
         {
            return phases_;
         }
         
         virtual void setPhases(const Phases& phases) 
         {
            phases_ = phases;
         }
        

      /// @name ZIP parameters:
      /// @{
      
         virtual const ublas::vector<Complex> YConst() const = 0;
         virtual const ublas::vector<Complex> IConst() const = 0;
         virtual const ublas::vector<Complex> SConst() const = 0;

      /// @}
      
      protected:

         virtual void print(std::ostream& os) const override;
      
      private:

         Phases phases_;
   };
}

#endif // ZIP_ABC_DOT_H
