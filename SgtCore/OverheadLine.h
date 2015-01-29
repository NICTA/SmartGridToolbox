#ifndef OVERHEAD_LINE_DOT_H
#define OVERHEAD_LINE_DOT_H

#include <SgtCore/Branch.h>

namespace SmartGridToolbox
{
   /// @brief An overhead power line.
   ///
   /// Consists of N + M wires, where N is the number of phases and M is the number of phases, and M is the number of
   /// extra grounded neutral wires that will be eliminated via the Kron reduction.
   class OverheadLine : public Component, public BranchAbc
   {
      public:

      SGT_PROPS_INIT(OverheadLine);
      SGT_PROPS_INHERIT(OverheadLine, Component);
      SGT_PROPS_INHERIT(OverheadLine, BranchAbc);

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
                      int nNeutral, arma::Col<double> lineResistivity, double earthResistivity,
                      arma::Mat<double> distMat, double freq);
 
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
         
         virtual arma::Mat<Complex> inServiceY() const override;
      
      /// @}
      
      /// @name Wire and phase impedance:
      /// @{
         
         arma::Mat<Complex> ZWire() const;

         arma::Mat<Complex> ZPhase(const arma::Mat<Complex>& ZWire) const;
         
      /// @}

      /// @name Printing:
      /// @{
         
         virtual void print(std::ostream& os) const
         {
            // TODO: proper printing.
            BranchAbc::print(os);
         }

      /// @}
      
      private:
         double L_;                      ///< Length.
         int nNeutral_;                  ///< Internal multigrounded neutral lines.
         arma::Col<double> rhoLine_; ///< Line resistivity.
         double rhoEarth_;               ///< Earth resistivity.
         arma::Mat<double> Dij_;     ///< Distance between lines, diagonal = GMR.
         double f_;                      ///< Frequency : TODO : link to network frequency.
   };
}

#endif // OVERHEAD_LINE_DOT_H
