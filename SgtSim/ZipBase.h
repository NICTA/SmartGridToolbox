#ifndef ZIP_TO_GROUND_BASE_DOT_H
#define ZIP_TO_GROUND_BASE_DOT_H

#include <SgtSim/Component.h>

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

namespace SmartGridToolbox
{
   /// @brief A load (or sometimes generator) with constant Z, I, P components.
   /// @ingroup PowerFlowCore
   class ZipBase : public Component
   {
      /// @name Overridden member functions from Simulated.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;
      
      /// @}

      /// @name My member functions.
      /// @{
      
      public:
         ZipBase(const std::string& name, const Phases& phases) : Component(name), phases_(phases)
         {
            // Empty.
         }

         virtual const Phases& phases() const {return phases_;}

         virtual ublas::vector<Complex> Y() const = 0;
         virtual ublas::vector<Complex> I() const = 0;
         virtual ublas::vector<Complex> S() const = 0;

      private:
         Phases phases_; ///< My phases on parent bus.
   };
}

#endif // ZIP_TO_GROUND_BASE_DOT_H
