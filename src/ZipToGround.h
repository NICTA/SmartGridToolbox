#ifndef ZIP_TO_GROUND_DOT_H
#define ZIP_TO_GROUND_DOT_H

#include "Parser.h"
#include "ZipToGroundBase.h"

namespace SmartGridToolbox
{
   class ZipToGroundParser : public ComponentParser
   {
      public:
         static constexpr const char * getComponentName()
         {
            return "zip_to_ground";
         }
      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod) const override;
   };

   class ZipToGround : public ZipToGroundBase
   {
      /// @name My public member functions.
      /// @{
      public:
         ZipToGround(const std::string & name) : ZipToGroundBase(name), Y_(0.0), I_(0.0), S_(0.0) {}

         virtual const UblasVector<Complex> & getY() const override {return Y_;}
         virtual void setY(const UblasVector<Complex> & Y) {Y_ = Y;}

         virtual const UblasVector<Complex> & getI() const override {return I_;} // Injection.
         virtual void setI(const UblasVector<Complex> & I) {I_ = I;} // Injection.

         virtual const UblasVector<Complex> & getS() const override {return S_;} // Injection.
         virtual void setS(const UblasVector<Complex> & S) {S_ = S;} // Injection.
      /// @}
      
      /// @name My private member variables.
      /// @{
         UblasVector<Complex> Y_;      ///< Constant admittance component.
         UblasVector<Complex> I_;      ///< Constant current injection component.
         UblasVector<Complex> S_;      ///< Constant power injection component.
      /// @}
   };
}

#endif // ZIP_TO_GROUND_DOT_H
