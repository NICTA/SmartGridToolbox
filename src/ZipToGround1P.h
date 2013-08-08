#ifndef ZIP_TO_GROUND_1P_DOT_H
#define ZIP_TO_GROUND_1P_DOT_H

#include "Component.h"
#include "Parser.h"

namespace SmartGridToolbox
{
   class ZipToGround1PParser : public ComponentParser
   {
      public:
         static constexpr const char * getComponentName()
         {
            return "zip_to_ground_1_phase";
         }
      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod) const override;
   };

   class ZipToGround1P : public Component
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
         ZipToGround1P(const std::string & name) : Component(name), Y_(0.0), I_(0.0), S_(0.0) {}

         const Complex & getY() const {return Y_;}
         void setY(const Complex & Y) {Y_ = Y;}

         const Complex & getI() const {return I_;} // Injection.
         void setI(const Complex & I) {I_ = I;} // Injection.

         const Complex & getS() const {return S_;} // Injection.
         void setS(const Complex & S) {S_ = S;} // Injection.
      /// @}
      
      /// @name My private member variables.
      /// @{
         Complex Y_;          ///< Constant admittance component.
         Complex I_;          ///< Constant current injection component.
         Complex S_;          ///< Constant power injection component.
      /// @}
   };
}

#endif // ZIP_TO_GROUND_1P_DOT_H
