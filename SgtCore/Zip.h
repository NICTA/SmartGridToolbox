#ifndef ZIP_DOT_H
#define ZIP_DOT_H

#include <SgtCore/PowerFlow.h>

#include<string>

namespace SmartGridToolbox
{
   /// @brief A Zip is an injection into a bus with constant impedance / current / complex power components.
   class Zip
   {
      public:
      
      /// @name Lifecycle:
      /// @{

         Zip(const std::string& id, Phases phases) :
            id_(id), phases_(phases), 
            YConst_(phases.size(), czero), IConst_(phases.size(), czero), SConst_(phases.size(), czero)
         {
            // Empty.
         }

         virtual ~Zip() = default;
      
      /// @}

      /// @name Basic identity and type:
      /// @{
 
         const std::string& id() const
         {
            return id_;
         }
         
         virtual void setId(const std::string& id)
         {
            id_ = id;
         }

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

         std::string id_;
         Phases phases_;

         ublas::vector<Complex> YConst_;
         ublas::vector<Complex> IConst_;
         ublas::vector<Complex> SConst_;
   };
}

#endif // ZIP_DOT_H
