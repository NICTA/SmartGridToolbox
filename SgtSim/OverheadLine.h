#ifndef OVERHEAD_LINE_DOT_H
#define OVERHEAD_LINE_DOT_H

#include <SgtSim/Branch.h>

namespace SmartGridToolbox
{
   /// @brief An overhead power line.
   ///
   /// Consists of N + M wires, where N is the number of phases and M is the number of phases, and M is the number of
   /// extra grounded neutral wires that will be eliminated via the Kron reduction.
   class OverheadLine : public Branch
   {
      /// @name Overridden member functions from Component.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;

      /// @}

      /// @name My public member functions.
      /// @{
         
      public:
         /// @brief Constructor.
         /// @param phases0 The phases on side A of the line.
         /// @param phases1 The phases on side B of the line.
         /// @param length The length in meters.
         /// @param nNeutral The number of extra neutral lines not explicitly included in the phases.
         /// @param lineResistivity The resistivity of each wire.
         /// @param earthResistivity The effective resistivity of the earth (typically 100 Ohm-meters).
         /// @param distMat Offdiagonal = distances between wires, diagonal = wire GMR.
         /// @param freq = the network frequency.
         OverheadLine(const std::string& name, const Phases& phases0, const Phases& phases1, double length,
                      int nNeutral, ublas::vector<double> lineResistivity, double earthResistivity,
                      ublas::matrix<double> distMat, double freq);

         const ublas::matrix<Complex>& ZWire()
         {
            return ZWire_;
         }

         const ublas::matrix<Complex>& ZPhase()
         {
            return ZPhase_;
         }

      /// @}
      
      /// @name My private member functions.
      /// @{
      
      private:
         void recalcY();

      /// @}
      
      private:
         double L_;                      ///< Length.
         int nNeutral_;                  ///< Internal multigrounded neutral lines.
         ublas::vector<double> rhoLine_; ///< Line resistivity.
         double rhoEarth_;               ///< Earth resistivity.
         ublas::matrix<double> Dij_;     ///< Distance between lines, diagonal = GMR.
         double f_;                      ///< Frequency : TODO : link to network frequency.

         ublas::matrix<Complex> ZWire_;  ///< Cached.
         ublas::matrix<Complex> ZPhase_; ///< Cached.
   };
}

#endif // OVERHEAD_LINE_DOT_H
