#ifndef ZIP_DOT_H
#define ZIP_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include<string>

namespace SmartGridToolbox
{
   /// @brief A Zip is an injection into a bus with constant impedance / current / complex power components.
   ///
   /// Implement some common functionality for convenience.
   class ZipAbc : public Component
   {
      public:

      /// @name Static member functions:
      /// @{
         
         static const std::string& sComponentType()
         {
            static std::string result("zip");
            return result;
         }
      
      /// @}
      
      /// @name Lifecycle:
      /// @{

         ZipAbc(const std::string& id, const Phases& phases) : Component(id), phases_(phases) {}

      /// @}
            
      /// @name ComponentInterface virtual overridden functions.
      /// @{
        
         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }

         virtual void print(std::ostream& os) const override;

      /// @}
         
      /// @name Phases
      /// @{
 
         virtual const Phases& phases() const
         {
            return phases_;
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
      ///
      /// These are implemented for convenience, so subclasses don't have to reimplement them unless they have
      /// a non-zero value.
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
         virtual Event& isInServiceChanged()
         {
            return isInServiceChanged_;
         }
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& injectionChanged()
         {
            return injectionChanged_;
         }

         /// @brief Event triggered when I go in or out of service.
         virtual Event& setpointChanged()
         {
            return setpointChanged_;
         }

      /// @}
     
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

      /// @name Static member functions:
      /// @{
         
         static const std::string& sComponentType()
         {
            static std::string result("generic_zip");
            return result;
         }
      
      /// @}
      
      /// @name Lifecycle:
      /// @{

         GenericZip(const std::string& id, const Phases& phases);

      /// @}
 
      /// @name ComponentInterface virtual overridden functions.
      /// @{
        
         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }

         // virtual void print(std::ostream& os) const override; TODO

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
