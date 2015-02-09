#ifndef GEN_DOT_H
#define GEN_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

namespace SmartGridToolbox
{
   /// @brief Common abstract base class for a generation at a bus.
   ///
   /// Implement some common functionality for convenience.
   class GenAbc : public Component
   {
      public:

      /// @name Static member functions:
      /// @{
         
         static const std::string& sComponentType()
         {
            static std::string result("gen");
            return result;
         }
      
      /// @}

      /// @name Lifecycle:
      /// @{
         
         GenAbc(const std::string& id, const Phases& phases) :
            Component(id),
            phases_(phases),
            isInService_(true)
         {
            // Empty.
         }

      /// @}

      /// @name ComponentInterface virtual overridden functions.
      /// @{
         
         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }

         virtual void print(std::ostream& os) const override;

      /// @}
         
      /// @name Phase accessors:
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
  
      /// @name Power injection:
      /// @{

         virtual arma::Col<Complex> S() const final
         {
            return isInService_ ? inServiceS() : arma::Col<Complex>(phases_.size());
         }

         virtual arma::Col<Complex> inServiceS() const = 0;

         virtual void setInServiceS(const arma::Col<Complex>& S) = 0; 

      /// @}
       
      /// @name Moment of inertia:
      /// @{

         virtual double J() const final
         {
            return isInService_ ? inServiceJ() : 0.0;
         }

         virtual double inServiceJ() const = 0;

      /// @}
     
      /// @name Generation bounds:
      /// @{

         virtual double PMin() const = 0;
         virtual void setPMin(double PMin) = 0;
         virtual double PMax() const = 0;
         virtual void setPMax(double PMax) = 0;
         virtual double QMin() const = 0;
         virtual void setQMin(double QMin) = 0;
         virtual double QMax() const = 0;
         virtual void setQMax(double QMax) = 0;

      /// @}
      
      /// @name Generation costs:
      /// @{
          
         virtual double cStartup() const = 0;
         virtual void setCStartup(double cStartup) = 0;
         virtual double cShutdown() const = 0;
         virtual void setCShutdown(double cShutdown) = 0;
         virtual double c0() const = 0;
         virtual void setC0(double c0) = 0;
         virtual double c1() const = 0;
         virtual void setC1(double c1) = 0;
         virtual double c2() const = 0;
         virtual void setC2(double c2) = 0;

         double cost() const;
   
      /// @}
    
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged()
         {
            return isInServiceChanged_;
         }
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& generationChanged()
         {
            return generationChanged_;
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
         Event generationChanged_{std::string(sComponentType()) + " : Generation changed"};
         Event setpointChanged_{std::string(sComponentType()) + " : Setpoint changed"};
   };
   
   /// @brief A concrete, generic generation at a bus.
   class GenericGen : public GenAbc
   {
      public:

      /// @name Static member functions:
      /// @{
         
         static const std::string& sComponentType()
         {
            static std::string result("generic_gen");
            return result;
         }
      
      /// @}

      /// @name Lifecycle:
      /// @{

         GenericGen(const std::string& id, const Phases& phases) :
            GenAbc(id, phases),
            S_(phases.size(), arma::fill::zeros)
         {
            // Empty.
         }

      /// @}

      /// @name ComponentInterface virtual overridden functions.
      /// @{

         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }

         // virtual void print(std::ostream& os) const override; // TODO

      /// @}

      /// @name Power injection:
      /// @{

         virtual arma::Col<Complex> inServiceS() const override
         {
            return S_;
         }

         virtual void setInServiceS(const arma::Col<Complex>& S) override
         {
            S_ = S;
         }
      
      /// @}
       
      /// @name Moment of inertia:
      /// @{

         virtual double inServiceJ() const override
         {
            return J_;
         }

         virtual void setInServiceJ(double J)
         {
            J_ = J;
         }

      /// @}
    
      /// @name Generation bounds:
      /// @{

         virtual double PMin() const override
         {
            return PMin_;
         }

         virtual void setPMin(double PMin) override
         {
            PMin_ = PMin;
         }
 
         virtual double PMax() const override
         {
            return PMax_;
         }

         virtual void setPMax(double PMax) override
         {
            PMax_ = PMax;
         }

         virtual double QMin() const override
         {
            return QMin_;
         }

         virtual void setQMin(double QMin) override
         {
            QMin_ = QMin;
         }
 
         virtual double QMax() const override
         {
            return QMax_;
         }

         virtual void setQMax(double QMax) override
         {
            QMax_ = QMax;
         }
          
      /// @}
      
      /// @name Generation costs:
      /// @{
          
         virtual double cStartup() const override
         {
            return cStartup_;
         }

         virtual void setCStartup(double cStartup) override
         {
            cStartup_ = cStartup;
         }
         
         virtual double cShutdown() const override
         {
            return cShutdown_;
         }

         virtual void setCShutdown(double cShutdown) override
         {
            cShutdown_ = cShutdown;
         }
       
         virtual double c0() const override
         {
            return c0_;
         }

         virtual void setC0(double c0) override
         {
            c0_ = c0;
         }
         
         virtual double c1() const override
         {
            return c1_;
         }

         virtual void setC1(double c1) override
         {
            c1_ = c1;
         }
         
         virtual double c2() const override
         {
            return c2_;
         }

         virtual void setC2(double c2) override
         {
            c2_ = c2;
         }
  
      private:

         arma::Col<Complex> S_;
         
         double J_{0.0};
         
         double PMin_{-infinity};
         double PMax_{infinity};
         double QMin_{-infinity};
         double QMax_{infinity};

         double cStartup_{0.0};
         double cShutdown_{0.0};
         double c0_{0.0};
         double c1_{0.0};
         double c2_{0.0};
   };
}

#endif // GEN_DOT_H
