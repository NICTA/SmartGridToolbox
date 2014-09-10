#ifndef OVERHEAD_LINE_DOT_H
#define OVERHEAD_LINE_DOT_H

#include <SgtCore/Branch.h>

namespace SmartGridToolbox
{
   /// @brief An overhead power line.
   ///
   /// Consists of N + M wires, where N is the number of phases and M is the number of phases, and M is the number of
   /// extra grounded neutral wires that will be eliminated via the Kron reduction.
   class OverheadLine : public BranchAbc
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         /// @brief Constructor.
         /// @param phases0 The phases on side A of the line.
         /// @param phases1 The phases on side B of the line.
         /// @param length The length in meters.
         /// @param nNeutral The number of extra neutral lines not explicitly included in the phases.
         /// @param lineResistivity The resistivity of each wire.
         /// @param earthResistivity The effective resistivity of the earth (typically 100 Ohm-meters).
         /// @param distMat Offdiagonal = distances between wires, diagonal = wire GMR.
         /// @param freq = the network frequency.
         OverheadLine(const std::string& id, const Phases& phases0, const Phases& phases1, double length,
                      int nNeutral, ublas::vector<double> lineResistivity, double earthResistivity,
                      ublas::matrix<double> distMat, double freq);
 
      /// @}
      
      /// @name Component Type:
      /// @{

         static constexpr const char* sComponentType()
         {
            return "overhead_line";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }

      /// @}
     
      /// @name Overridden from BranchAbc:
      /// @{
         
         virtual const ublas::matrix<Complex> Y() const override;
      
      /// @}
      
      private:
         double L_;                      ///< Length.
         int nNeutral_;                  ///< Internal multigrounded neutral lines.
         ublas::vector<double> rhoLine_; ///< Line resistivity.
         double rhoEarth_;               ///< Earth resistivity.
         ublas::matrix<double> Dij_;     ///< Distance between lines, diagonal = GMR.
         double f_;                      ///< Frequency : TODO : link to network frequency.
   };
}

#endif // OVERHEAD_LINE_DOT_H
