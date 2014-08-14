#ifndef GEN_DOT_H
#define GEN_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/PowerFlow.h>

namespace SmartGridToolbox
{
   /// @brief A generation at a bus.
   class Gen : public Component
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         Gen(const std::string& id, Phases phases);

         virtual ~Gen() = default;
      
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
         
         bool status() const
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

         const ublas::vector<Complex>& S() const
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

         double PMin() const
         {
            return PMin_;
         }

         virtual void setPMin(double PMin)
         {
            PMin_ = PMin;
         }
 
         double PMax() const
         {
            return PMax_;
         }

         virtual void setPMax(double PMax)
         {
            PMax_ = PMax;
         }

         double QMin() const
         {
            return QMin_;
         }

         virtual void setQMin(double QMin)
         {
            QMin_ = QMin;
         }
 
         double QMax() const
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
          
         double cStartup() const
         {
            return cStartup_;
         }

         virtual void setCStartup(double cStartup)
         {
            cStartup_ = cStartup;
         }
         
         virtual double cShutdown() const
         {
            return cShutdown_;
         }

         void setCShutdown(double cShutdown)
         {
            cShutdown_ = cShutdown;
         }
       
         double c0() const
         {
            return c0_;
         }

         virtual void setC0(double c0)
         {
            c0_ = c0;
         }
         
         double c1() const
         {
            return c1_;
         }

         virtual void setC1(double c1)
         {
            c1_ = c1;
         }
         
         double c2() const
         {
            return c2_;
         }

         virtual void setC2(double c2)
         {
            c2_ = c2;
         }
   
      /// @}
      
      /// @name Output
      /// @{
         
         friend std::ostream& operator<<(std::ostream& os, const Gen& gen)
         {
            return gen.print(os);
         }

      /// @}

      protected:

         virtual std::ostream& print(std::ostream& os) const;
      
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
