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
      /// @name Overridden public member functions from ZipToGroundBase.
      /// @{
      public:
         virtual UblasVector<Complex> getY() const override {return Y_;}
         virtual UblasVector<Complex> getI() const override {return I_;} // Injection.
         virtual UblasVector<Complex> getS() const override {return S_;} // Injection.
      /// @}

      /// @name My public member functions.
      /// @{
      public:
         ZipToGround(const std::string & name) : ZipToGroundBase(name), Y_(0.0), I_(0.0), S_(0.0) {}

         // Add non-const reference accessors:
         virtual UblasVector<Complex> & getY() {return Y_;}
         virtual UblasVector<Complex> & getI() {return I_;} // Injection.
         virtual UblasVector<Complex> & getS() {return S_;} // Injection.
      /// @}
      
      /// @name My private member variables.
      /// @{
      private:
         UblasVector<Complex> Y_;      ///< Constant admittance component.
         UblasVector<Complex> I_;      ///< Constant current injection component.
         UblasVector<Complex> S_;      ///< Constant power injection component.
      /// @}
   };
}

#endif // ZIP_TO_GROUND_DOT_H
