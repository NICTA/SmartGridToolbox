#ifndef ZIP_DOT_H
#define ZIP_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include<string>

namespace SmartGridToolbox
{
   /// @brief Common abstract base class for a ZIP at a bus.
   ///
   /// Implement some common functionality for convenience.
   class ZipAbc : public Component
   {
      public:
      
      /// @name Lifecycle:
      /// @{

         ZipAbc(const std::string& id, Phases phases) : Component(id), phases_(phases) {}

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
      
      /// @}

      /// @name In service:
      /// @{
         
         virtual bool isInService() const
         {
            return isInService_;
         }

         virtual void setIsInService(bool isInService)
         {
            isInService_ = isInService;
         }

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

         bool isInService_;
         
         Event isInServiceChanged_;
         Event injectionChanged_;
         Event setpointChanged_;
   };

   /// @brief A concrete, generic ZIP at a bus.
   class GenericZip : public ZipAbc
   {
      public:
      
      /// @name Lifecycle:
      /// @{

         GenericZip(const std::string& id, Phases phases);

      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentType() const {return "generic_zip";}

      /// @}
            
      /// @name ZIP parameters:
      /// @{
      
         virtual ublas::vector<Complex> YConst() const
         {
            return YConst_;
         }
         
         virtual void setYConst(const ublas::vector<Complex>& YConst)
         {
            YConst_ = YConst;
         }
      
         virtual ublas::vector<Complex> IConst() const
         {
            return IConst_;
         }
         
         virtual void setIConst(const ublas::vector<Complex>& IConst)
         {
            IConst_ = IConst;
         }
      
         virtual ublas::vector<Complex> SConst() const
         {
            return SConst_;
         }
         
         virtual void setSConst(const ublas::vector<Complex>& SConst)
         {
            SConst_ = SConst;
         }

      /// @}
         
      private:

         Phases phases_;

         ublas::vector<Complex> YConst_;
         ublas::vector<Complex> IConst_;
         ublas::vector<Complex> SConst_;
   };
}

#endif // ZIP_DOT_H
