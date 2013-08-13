#ifndef ZIP_TO_GROUND_DOT_H
#define ZIP_TO_GROUND_DOT_H

#include "Component.h"
#include "Parser.h"

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

   class ZipToGround : public Component
   {
      /// @name Public overridden functions: from Component.
      /// @{
      public:
         virtual ptime getValidUntil() const override 
         {
            return pos_infin;
         }
      /// @}

      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState(ptime t) override {};
         virtual void updateState(ptime t0, ptime t1) override {};
      /// @}
      
      /// @name My public member functions.
      /// @{
      public:
         ZipToGround(const std::string & name) : Component(name), Y_(0.0), I_(0.0), S_(0.0) {}

         const std::vector<Phase> & getPhases() const {return phases_;}
         void setPhases(const std::vector<Phase> & phases) {phases_ = phases;}

         const UblasVector<Complex> & getY() const {return Y_;}
         void setY(const UblasVector<Complex> & Y) {Y_ = Y;}

         const UblasVector<Complex> & getI() const {return I_;} // Injection.
         void setI(const UblasVector<Complex> & I) {I_ = I;} // Injection.

         const UblasVector<Complex> & getS() const {return S_;} // Injection.
         void setS(const UblasVector<Complex> & S) {S_ = S;} // Injection.
      /// @}
      
      /// @name My private member variables.
      /// @{
         std::vector<Phase> phases_;   ///< My phases on parent bus.
         UblasVector<Complex> Y_;      ///< Constant admittance component.
         UblasVector<Complex> I_;      ///< Constant current injection component.
         UblasVector<Complex> S_;      ///< Constant power injection component.
      /// @}
   };
}

#endif // ZIP_TO_GROUND_DOT_H
