#ifndef GEN_DOT_H
#define GEN_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/GenAbc.h>
#include <SgtCore/PowerFlow.h>

namespace SmartGridToolbox
{
   /// @brief A generation at a bus.
   class Gen : public GenAbc
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         Gen(const std::string& id, Phases phases);

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
         
         virtual bool status() const override
         {
            return status_;
         }

         virtual void setStatus(bool status)
         {
            status_ = status;
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

         Phases phases_;
         bool status_;
         
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
