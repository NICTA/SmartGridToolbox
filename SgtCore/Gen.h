#ifndef GEN_DOT_H
#define GEN_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

namespace SmartGridToolbox
{
   /// @brief Abstract interface for a generation at a bus.
   class GenInterface : virtual public ComponentInterface
   {
      public:
      /// @brief Lifecycle:
      /// @{

         virtual ~GenInterface() {}
      
      /// @}

      /// @brief Phase accessors:
      /// @{
         
         virtual const Phases& phases() const = 0;
        
      /// @}
 
      /// @name In service:
      /// @{
         
         virtual bool isInService() const = 0;
         virtual void setIsInService(bool isInService) = 0;

      /// @}
      
      /// @name Power injection:
      /// @{

         virtual const ublas::vector<Complex>& S() const = 0;
         virtual void setS(const ublas::vector<Complex>& S) = 0;

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
   
      /// @}
      
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged() = 0;
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& generationChanged() = 0;

         /// @brief Event triggered when I go in or out of service.
         virtual Event& setpointChanged() = 0;

      /// @}
      
      /// @name Printing.
      /// @{
      
      protected:
         
         virtual void print(std::ostream& os) const;

      /// @}
   };

   /// @brief Common abstract base class for a generation at a bus.
   ///
   /// Implement some common functionality for convenience.
   class GenAbc : virtual public GenInterface, public Component
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         GenAbc(const std::string& id, Phases phases);

      /// @}

      /// @name Phase accessors:
      /// @{
         
         virtual const Phases& phases() const
         {
            return phases_;
         }
         
         virtual void setPhases(const Phases& phases) 
         {
            phases_ = phases;
         }
        
      /// @}
 
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged() override {return isInServiceChanged_;}
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& generationChanged() override {return generationChanged_;}

         /// @brief Event triggered when I go in or out of service.
         virtual Event& setpointChanged() override {return setpointChanged_;}

      /// @}
      
      
      private:

         Phases phases_;

         Event isInServiceChanged_;
         Event generationChanged_;
         Event setpointChanged_;
   };
   
   /// @brief A concrete, generic generation at a bus.
   class GenericGen : public GenAbc
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         GenericGen(const std::string& id, Phases phases);

      /// @}

      /// @{
         
         virtual const char* componentTypeStr() const {return "generic_gen";}

      /// @}

      /// @name In service:
      /// @{
         
         virtual bool isInService() const override
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

         virtual const ublas::vector<Complex>& S() const override
         {
            return S_;
         }

         virtual void setS(const ublas::vector<Complex>& S)
         {
            S_ = S;
         }
      
      /// @}
      
      /// @name Generation bounds:
      /// @{

         virtual double PMin() const override
         {
            return PMin_;
         }

         virtual void setPMin(double PMin)
         {
            PMin_ = PMin;
         }
 
         virtual double PMax() const override
         {
            return PMax_;
         }

         virtual void setPMax(double PMax)
         {
            PMax_ = PMax;
         }

         virtual double QMin() const override
         {
            return QMin_;
         }

         virtual void setQMin(double QMin)
         {
            QMin_ = QMin;
         }
 
         virtual double QMax() const override
         {
            return QMax_;
         }

         virtual void setQMax(double QMax)
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

         virtual void setCStartup(double cStartup)
         {
            cStartup_ = cStartup;
         }
         
         virtual double cShutdown() const override
         {
            return cShutdown_;
         }

         virtual void setCShutdown(double cShutdown)
         {
            cShutdown_ = cShutdown;
         }
       
         virtual double c0() const override
         {
            return c0_;
         }

         virtual void setC0(double c0)
         {
            c0_ = c0;
         }
         
         virtual double c1() const override
         {
            return c1_;
         }

         virtual void setC1(double c1)
         {
            c1_ = c1;
         }
         
         virtual double c2() const override
         {
            return c2_;
         }

         virtual void setC2(double c2)
         {
            c2_ = c2;
         }
   
      /// @}
      
      private:

         bool isInService_;
         
         ublas::vector<Complex> S_;
         
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
