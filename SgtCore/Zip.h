#ifndef ZIP_DOT_H
#define ZIP_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/PowerFlow.h>

#include<string>

namespace SmartGridToolbox
{
   /// @brief A Zip is an injection into a bus with constant impedance / current / complex power components.
   class Zip : public Component
   {
      public:
      
      /// @name Lifecycle:
      /// @{

         Zip(const std::string& id, Phases phases);

         virtual ~Zip() = default;
      
      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentTypeStr() const {return "zip";}

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
      
         const ublas::vector<Complex> YConst() const
         {
            return YConst_;
         }
         
         virtual void setYConst(const ublas::vector<Complex>& YConst)
         {
            YConst_ = YConst;
         }
      
         const ublas::vector<Complex> IConst() const
         {
            return IConst_;
         }
         
         virtual void setIConst(const ublas::vector<Complex>& IConst)
         {
            IConst_ = IConst;
         }
      
         const ublas::vector<Complex> SConst() const
         {
            return SConst_;
         }
         
         virtual void setSConst(const ublas::vector<Complex>& SConst)
         {
            SConst_ = SConst;
         }

      /// @}
         
      /// @name Output
      /// @{
         
         friend std::ostream& operator<<(std::ostream& os, const Zip& zip)
         {
            return zip.print(os);
         }
      
      /// @}
      
      protected:

         virtual std::ostream& print(std::ostream& os) const;
      
      private:

         Phases phases_;

         ublas::vector<Complex> YConst_;
         ublas::vector<Complex> IConst_;
         ublas::vector<Complex> SConst_;
   };
}

#endif // ZIP_DOT_H
