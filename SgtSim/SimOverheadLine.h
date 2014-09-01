#ifndef SIM_OVERHEAD_LINE_DOT_H
#define SIM_OVERHEAD_LINE_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/OverheadLine.h>

namespace SmartGridToolbox
{
   class SimOverheadLine : public OverheadLine, public SimComponent
   {
      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;

      /// @}

      public:
         SimOverheadLine(const std::string& id, const Phases& phases0, const Phases& phases1, double length,
                      int nNeutral, ublas::vector<double> lineResistivity, double earthResistivity,
                      ublas::matrix<double> distMat, double freq) :
            OverheadLine(id, phases0, phases1, length, nNeutral, lineResistivity, earthResistivity, distMat, freq)
         {
            // Empty.
         }

         SimOverheadLine(const OverheadLine& ohl) : OverheadLine(ohl)
         {
            // Empty.
         }

      /// @}
   };
}

#endif // SIM_OVERHEAD_LINE_DOT_H
