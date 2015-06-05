// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef UNDERGROUND_LINE_DOT_H
#define UNDERGROUND_LINE_DOT_H

#include <SgtCore/Branch.h>

namespace Sgt
{
    /// @brief An underground power line.
    ///
    /// As in Gridlab-D, we take direction from Kersting: Distribution System Modelling and Analysis.
    ///
    /// Phases A, B, and C are individually surrounded by *either* a concentric stranded neutral, or a neutral
    /// tape shield. In addition, there may be a separate neutral wire, which does not have its own shielding as it is
    /// not expected to carry (much) current.
    /// Thus, there are either 6 or 7 conductors to consider, depending on whether there is a separate neutral wire.
    /// Conductors 1-3 and 7 are the usual type of conductor with their own particular GMR specified, etc.
    /// Conductors 4-6 represent either the concentric neutrals or the tape.
    /// @ingroup PowerFlow
    class UndergroundLine : public BranchAbc
    {
        public:

            SGT_PROPS_INIT(UndergroundLine);
            SGT_PROPS_INHERIT(UndergroundLine, Component);
            SGT_PROPS_INHERIT(UndergroundLine, BranchAbc);

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("underground_line");
                return result;
            }

        /// @}

        /// @name Lifecycle:
        /// @{

            /// @brief Constructor.
            UndergroundLine(const std::string& id, double L, bool hasNeutral, const arma::Mat<double>& phaseDist,
                            double gmrPhase, double resPerLPhase, double freq, double rhoEarth);

            virtual void validate() = 0; ///< Calcuate all cached quantities.

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

        /// @name UndergroundLine specific member functions
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

        protected:

            // Line Parameters:

            double L_; ///< Line length.
            bool hasNeutral_{false}; ///< Is there a separate (unshielded) neutral wire.
            arma::Mat<double> phaseDist_; ///< Distance between phases (possibly including neutral).

            double gmrPhase_; ///< GMR of phase conductor, GMR_c in Kersting.
            double resPerLPhase_; ///< resistance/length of phase conductor, r_c in Kersting.

            double freq_; ///< Frequency : TODO : link to network frequency.
            double rhoEarth_{100.0}; ///< Earth resistivity.

            // Cached quantities:

            arma::Mat<Complex> ZPrim_; ///< Primative line impedance matrix.
            arma::Mat<Complex> ZPhase_; ///< Phase line impedance matrix.
            arma::Mat<Complex> YNode_; ///< Nodal admittance matrix.
    };

    /// @brief Underground line with stranded shielding.
    /// @ingroup PowerFlow
    class UndergroundLineStrandedShield : public UndergroundLine
    {
        public:
            
            UndergroundLineStrandedShield(
                    const std::string& id, double L, bool hasNeutral, const arma::Mat<double>& phaseDist,
                    double gmrPhase, double resPerLPhase, double freq, double rhoEarth,
                    double gmrConcStrand, double resPerLConcStrand, int nConcStrands, double rConc);
            
            virtual void validate() override; ///< Calcuate all cached quantities.

        private:

            double gmrConcStrand_; ///< GMR of concentric neutral strand, GMR_s in Kersting.
            double resPerLConcStrand_; ///< resistance/length of concentric neutral strand, r_s in Kersting.
            int nConcStrands_; ///< Number of concentric neutral strands, k in Kersting.
            double rConc_; ///< Radius center of phase conductor to center of shield strands, R in Kersting.
    };
}

#endif // UNDERGROUND_LINE_DOT_H
