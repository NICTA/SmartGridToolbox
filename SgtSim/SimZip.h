#ifndef ZIP_COMP_DOT_H
#define ZIP_COMP_DOT_H

#include <SgtSim/ZipBase.h>

namespace SmartGridToolbox
{
   /// @brief A load (or sometimes generator) with constant Z, I, P components.
   /// @ingroup PowerFlowCore
   class SimZip : public ZipBase
   {
      /// @name Overridden member functions from Component.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;
      
      /// @}

      /// @name Overridden member functions from ZipBase.
      /// @{
      
      public:
         virtual ublas::vector<Complex> Y() const override {return Y_;}
         virtual ublas::vector<Complex> I() const override {return I_;} // Injection.
         virtual ublas::vector<Complex> S() const override {return S_;} // Injection.

      /// @}
      
      /// @name My member functions.
      /// @{

      public:
         SimZip(const std::string& name, const Phases& phases);

         // Add non-const reference accessors:
         virtual ublas::vector<Complex>& Y() {return Y_;}
         virtual ublas::vector<Complex>& I() {return I_;} // Injection.
         virtual ublas::vector<Complex>& S() {return S_;} // Injection.
 
      /// @}
      
      private:
         ublas::vector<Complex> Y_; ///< Constant admittance component.
         ublas::vector<Complex> I_; ///< Constant current injection component.
         ublas::vector<Complex> S_; ///< Constant power injection component.
   };
}

#endif // ZIP_COMP_DOT_H
