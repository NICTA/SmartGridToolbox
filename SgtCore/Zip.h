#ifndef ZIP_DOT_H
#define ZIP_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include<string>

namespace SmartGridToolbox
{
   /// @brief A Zip is an injection into a bus with constant impedance / current / complex power components.
   class ZipInterface : virtual public ComponentInterface
   {
      public:
      
      /// @name Lifecycle:
      /// @{

         virtual ~ZipInterface() {}

      /// @}

      /// @name Basic identity and type:
      /// @{
 
         virtual const Phases& phases() const = 0;
         
      /// @name In service:
      /// @{
         
         virtual bool isInService() const = 0;
         virtual void setIsInService(bool isInService) = 0;

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
         virtual Event& isInServiceChanged() = 0;
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& injectionChanged() = 0;

         /// @brief Event triggered when I go in or out of service.
         virtual Event& setpointChanged() = 0;

      /// @}
   };

   /// @brief Common abstract base class for a ZIP at a bus.
   ///
   /// Implement some common functionality for convenience.
   class ZipAbc : public Component, virtual public ZipInterface
   {
      public:
      
      /// @name Lifecycle:
      /// @{

         ZipAbc(const std::string& id, Phases phases) : Component(id), phases_(phases) {}

      /// @}

      /// @name Basic identity and type:
      /// @{
 
         virtual const Phases& phases() const override
         {
            return phases_;
         }
         
      /// @}

      /// @name In service:
      /// @{
         
         virtual bool isInService() const override
         {
            return isInService_;
         }

         virtual void setIsInService(bool isInService) override
         {
            isInService_ = isInService;
         }

      /// @}
              
      /// @name ZIP parameters:
      /// @{
         
         virtual ublas::vector<Complex> YConst() const
         {
            return ublas::vector<Complex>(phases_.size(), czero);
         }

         virtual ublas::vector<Complex> IConst() const
         {
            return ublas::vector<Complex>(phases_.size(), czero);
         }

         virtual ublas::vector<Complex> SConst() const
         {
            return ublas::vector<Complex>(phases_.size(), czero);
         }

      /// @}

      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged() override
         {
            return isInServiceChanged_;
         }
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& injectionChanged() override
         {
            return injectionChanged_;
         }

         /// @brief Event triggered when I go in or out of service.
         virtual Event& setpointChanged() override
         {
            return setpointChanged_;
         }

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
         
         static constexpr const char* sComponentType()
         {
            return "generic_zip";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }

      /// @}
            
      /// @name ZIP parameters:
      /// @{
      
         virtual ublas::vector<Complex> YConst() const override
         {
            return YConst_;
         }
         
         virtual void setYConst(const ublas::vector<Complex>& YConst)
         {
            YConst_ = YConst;
         }
      
         virtual ublas::vector<Complex> IConst() const override
         {
            return IConst_;
         }
         
         virtual void setIConst(const ublas::vector<Complex>& IConst)
         {
            IConst_ = IConst;
         }
      
         virtual ublas::vector<Complex> SConst() const override
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
