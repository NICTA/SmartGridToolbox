#ifndef OVERHEAD_LINE_DOT_H
#define OVERHEAD_LINE_DOT_H

#include <SgtCore/Branch.h>

namespace Sgt
{
   /// @brief An overhead power line.
   ///
   /// As in Gridlab-D, we take direction from Kersting: Distribution System Modelling and Analysis.
   ///
   /// Phases A, B, and C are individually surrounded by *either* a concentric stranded neutral, or a neutral
   /// tape shield. In addition, there may be a separate neutral wire, which does not have its own shielding as it is
   /// not expected to carry (much) current.
   /// Thus, there are either 6 or 7 conductors to consider, depending on whether there is a separate neutral wire.
   /// Conductors 1-3 and 7 are the usual type of conductor with their own particular GMR specified, etc.
   /// Conductors 4-6 represent either the concentric neutrals or the tape.
   class OverheadLine : public BranchAbc
   {
      public:

         SGT_PROPS_INIT(OverheadLine);
         SGT_PROPS_INHERIT(OverheadLine, Component);
         SGT_PROPS_INHERIT(OverheadLine, BranchAbc);

      /// @name Static member functions:
      /// @{

         static const std::string& sComponentType()
         {
            static std::string result("overhead_line");
            return result;
         }

      /// @}

      /// @name Lifecycle:
      /// @{

         /// @brief Constructor.
         OverheadLine(const std::string& id, const Phases& phases0, const Phases& phases1, double L,
                      const arma::Mat<double>& Dij, const arma::Col<double>& resPerL,
                      double rhoEarth, double freq);
      /// @}

      /// @name ComponentInterface virtual overridden functions.
      /// @{

         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }

         // virtual void print(std::ostream& os) const override; // TODO

      /// @}

      /// @name Overridden from BranchAbc:
      /// @{

         virtual arma::Mat<Complex> inServiceY() const override
         {
            return YNode_;
         }

      /// @}

      /// @name OverheadLine specific member functions
      /// @{

         /// Primative line impedance matrix (before Kron).
         const arma::Mat<Complex>& ZPrim() const
         {
            return ZPrim_;
         }

         /// Phase line impedance matrix (after Kron).
         const arma::Mat<Complex>& ZPhase() const
         {
            return ZPhase_;
         }

      /// @}

      private:

      /// @name Private member functions
      /// @{

         void validate(); ///< Calcuate all cached quantities.

      /// @}

      private:

         // Line Parameters:

         double L_; ///< Line length.
         arma::Mat<double> Dij_; ///< Distance between wires on off diag, GMR of wides on diag.
         arma::Col<double> resPerL_; ///< resistance/length of each wire.
         double rhoEarth_{100.0}; ///< Earth resistivity.
         double freq_; ///< Frequency : TODO : link to network frequency.

         // Cached quantities:

         arma::Mat<Complex> ZPrim_; ///< Primative line impedance matrix.
         arma::Mat<Complex> ZPhase_; ///< Phase line impedance matrix.
         arma::Mat<Complex> YNode_; ///< Nodal admittance matrix.
   };
}

#endif // OVERHEAD_LINE_DOT_H
