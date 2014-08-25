#ifndef ZIP_DOT_H
#define ZIP_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/PowerFlow.h>
#include <SgtCore/ZipAbc.h>

#include<string>

namespace SmartGridToolbox
{
   /// @brief A Zip is an injection into a bus with constant impedance / current / complex power components.
   class Zip : public ZipAbc
   {
      public:
      
      /// @name Lifecycle:
      /// @{

         Zip(const std::string& id, Phases phases);

      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentType() const {return "zip";}

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
        

      /// @name ZIP parameters:
      /// @{
      
         virtual const ublas::vector<Complex> YConst() const override
         {
            return YConst_;
         }
         
         virtual void setYConst(const ublas::vector<Complex>& YConst)
         {
            YConst_ = YConst;
         }
      
         virtual const ublas::vector<Complex> IConst() const override
         {
            return IConst_;
         }
         
         virtual void setIConst(const ublas::vector<Complex>& IConst)
         {
            IConst_ = IConst;
         }
      
         virtual const ublas::vector<Complex> SConst() const override
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
