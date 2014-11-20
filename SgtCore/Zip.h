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
         
      /// @name Component Type:
      /// @{
         
         static constexpr const char* sComponentType()
         {
            return "zip";
         }

      /// @}

      /// @name Phases:
      /// @{
 
         virtual const Phases& phases() const = 0;
      
      /// @}
         
      /// @name In service:
      /// @{
         
         virtual bool isInService() const = 0;
         virtual void setIsInService(bool isInService) = 0;

      /// @}
      
      /// @name ZIP parameters:
      /// @{
      
         virtual arma::Col<Complex> YConst() const = 0;
         virtual arma::Col<Complex> IConst() const = 0;
         virtual arma::Col<Complex> SConst() const = 0;

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

         ZipAbc(const std::string& id, const Phases& phases) : Component(id), phases_(phases) {}

      /// @}
         
      /// @name Component Type:
      /// @{
         
         virtual const char* componentType() const override
         {
            return sComponentType();
         }
      
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
         
         virtual arma::Col<Complex> YConst() const
         {
            return arma::Col<Complex>(phases_.size(), arma::fill::zeros);
         }

         virtual arma::Col<Complex> IConst() const
         {
            return arma::Col<Complex>(phases_.size(), arma::fill::zeros);
         }

         virtual arma::Col<Complex> SConst() const
         {
            return arma::Col<Complex>(phases_.size(), arma::fill::zeros);
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
         
         Event isInServiceChanged_{std::string(sComponentType()) + " : Is in service changed"};
         Event injectionChanged_{std::string(sComponentType()) + " : Injection changed"};
         Event setpointChanged_{std::string(sComponentType()) + " : Setpoint changed"};
   };

   /// @brief A concrete, generic ZIP at a bus.
   class GenericZip : public ZipAbc
   {
      public:
      
      /// @name Lifecycle:
      /// @{

         GenericZip(const std::string& id, const Phases& phases);

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
      
         virtual arma::Col<Complex> YConst() const override
         {
            return YConst_;
         }
         
         virtual void setYConst(const arma::Col<Complex>& YConst)
         {
            YConst_ = YConst;
         }
      
         virtual arma::Col<Complex> IConst() const override
         {
            return IConst_;
         }
         
         virtual void setIConst(const arma::Col<Complex>& IConst)
         {
            IConst_ = IConst;
         }
      
         virtual arma::Col<Complex> SConst() const override
         {
            return SConst_;
         }
         
         virtual void setSConst(const arma::Col<Complex>& SConst)
         {
            SConst_ = SConst;
         }

      /// @}
         
      private:

         Phases phases_;

         arma::Col<Complex> YConst_;
         arma::Col<Complex> IConst_;
         arma::Col<Complex> SConst_;
   };
}

#endif // ZIP_DOT_H
