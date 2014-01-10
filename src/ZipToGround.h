#ifndef ZIP_TO_GROUND_DOT_H
#define ZIP_TO_GROUND_DOT_H

#include <SmartGridToolbox/ZipToGroundBase.h>

namespace SmartGridToolbox
{
   class ZipToGround : public ZipToGroundBase
   {
      /// @name Overridden public member functions from ZipToGroundBase.
      /// @{
      public:
         virtual ublas::vector<Complex> Y() const override {return Y_;}
         virtual ublas::vector<Complex> I() const override {return I_;} // Injection.
         virtual ublas::vector<Complex> S() const override {return S_;} // Injection.
      /// @}

      /// @name My public member functions.
      /// @{
      public:
         ZipToGround(const std::string & name, const Phases & phases);

         // Add non-const reference accessors:
         virtual ublas::vector<Complex> & Y() {return Y_;}
         virtual ublas::vector<Complex> & I() {return I_;} // Injection.
         virtual ublas::vector<Complex> & S() {return S_;} // Injection.
      /// @}
      
      /// @name My private member variables.
      /// @{
      private:
         ublas::vector<Complex> Y_; ///< Constant admittance component.
         ublas::vector<Complex> I_; ///< Constant current injection component.
         ublas::vector<Complex> S_; ///< Constant power injection component.
      /// @}
   };
}

#endif // ZIP_TO_GROUND_DOT_H
