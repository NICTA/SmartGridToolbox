#ifndef GEN_DOT_H
#define GEN_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

namespace SmartGridToolbox
{
   /// @brief A generation at a bus.
   class Gen
   {
      public:
      
      /// @name Lifecycle:
      /// @{
         
         Gen(const std::string& id, Phases phases) :
            id_(id), phases_(phases), status_(true), S_(phases.size(), czero)
         {
            // Empty.
         }

         virtual ~Gen()
         {
            // Empty.
         }
      
      /// @}

      /// @name Id:
      /// @{
 
         const std::string& id() const
         {
            return id_;
         }

         void setId(const std::string& id)
         {
            id_ = id;
         }

      /// @}
 
      /// @name Phase accessors:
      /// @{
         
         const Phases& phases() const
         {
            return phases_;
         }
         
         void setPhases(const Phases& phases) 
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

         void setStatus(bool status)
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

         void setS(const ublas::vector<Complex>& S)
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

         void setPMin(double PMin)
         {
            PMin_ = PMin;
         }
 
         double PMax() const
         {
            return PMax_;
         }

         void setPMax(double PMax)
         {
            PMax_ = PMax;
         }

         double QMin() const
         {
            return QMin_;
         }

         void setQMin(double QMin)
         {
            QMin_ = QMin;
         }
 
         double QMax() const
         {
            return QMax_;
         }

         void setQMax(double QMax)
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

         void setCStartup(double cStartup)
         {
            cStartup_ = cStartup;
         }
         
         double cShutdown() const
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

         void setC0(double c0)
         {
            c0_ = c0;
         }
         
         double c1() const
         {
            return c1_;
         }

         void setC1(double c1)
         {
            c1_ = c1;
         }
         
         double c2() const
         {
            return c2_;
         }

         void setC2(double c2)
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

         std::string id_;
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
