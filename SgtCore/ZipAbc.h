#ifndef ZIP_ABC_DOT_H
#define ZIP_ABC_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
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
        
      /// @name In service:
      /// @{
         
         virtual bool isInService() const = 0;

      /// @}
      
      /// @name ZIP parameters:
      /// @{
      
         virtual ublas::vector<Complex> YConst() const = 0;
         virtual ublas::vector<Complex> IConst() const = 0;
         virtual ublas::vector<Complex> SConst() const = 0;

      /// @}
       
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged() {return isInServiceChanged_;}
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& injectionChanged() {return injectionChanged_;}

         /// @brief Event triggered when I go in or out of service.
         virtual Event& setpointChanged() {return setpointChanged_;}

      /// @}
     
      protected:

         virtual void print(std::ostream& os) const override;
      
      private:

         Phases phases_;
         
         Event isInServiceChanged_;
         Event injectionChanged_;
         Event setpointChanged_;
   };
}

#endif // ZIP_ABC_DOT_H
