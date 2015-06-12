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

#ifndef OVERHEAD_LINE_DOT_H
#define OVERHEAD_LINE_DOT_H

#include <SgtCore/Branch.h>

namespace Sgt
{
    /// @brief An overhead power line.
    ///
    /// An overhead line with nCond conductors, with nPhase of the conductors corresponding to
    /// explicitly modelled phases (possibly including an explicit neutral phase), and the remaining
    /// (nCond - nPhase) conductors being grounded neutral wires that influence the inductance of the line
    /// but are not explicitly modelled as phases in the network. The conductors themselves may (or may not)
    /// consist of bundles of two, three or four single wires, usually separated by a small air gap. 
    /// For bundled wires, a bundling distance is defined which is the distance between the wires
    /// for two or three bundle wires (assumed to be in equilateral triangle in the latter case), or
    /// the side length of a square configuration for four bundled wires.
    /// As in Gridlab-D, we take direction from Kersting: Distribution System Modelling and Analysis.
    /// @ingroup PowerFlow
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
            /// @param id The line id. 
            /// @param phases0 The phases on the '0' (from) end.
            /// @param phases1 The phases on the '1' (to) end.
            /// @param L The line length in m.
            /// @param condDist Matrix with conductor spacing. Symmetric / zero diagonal.
            /// @param subcondGmr Matrix with conductor spacing on off diagonal, subconductor GMR on diagonal.
            /// @param subcondRPerL resistance per unit length of each subconductor.
            /// @param freq The nominal frequency of the system.
            /// @param rhoEarth The earth resistivity.
            /// @param nInBundle The number of bundled subconductors for each conductor (optional).
            /// @param adjSubcondDist The distance between adjacent bundled subconductors for each conductor (optional).
            OverheadLine(const std::string& id, const Phases& phases0, const Phases& phases1, double L,
                    const arma::Mat<double>& condDist, const arma::Mat<double> subcondGmr,
                    const arma::Col<double>& subcondRPerL, double freq = 50.0, double rhoEarth = 100.0,
                    const arma::Col<unsigned int>& nInBundle = arma::Col<unsigned int>(),
                    const arma::Col<double>& adjSubcondDist = arma::Col<double>());
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
            arma::Mat<double> condDist_; ///< Distance between conductors on off diag.
            arma::Col<double> subcondGmr_; ///< GMR of conductor, or each subconductor if bundled.
            arma::Col<double> subcondRPerL_; ///< resistance/length of each subconductor.
            double freq_{50.0}; ///< Frequency (Hz) : TODO : link to network frequency.
            double rhoEarth_{100.0}; ///< Earth resistivity.
            arma::Col<unsigned int> nInBundle_; ///< Number of bundled subconductors on each conductor.
            arma::Col<double> adjSubcondDist_; ///< Bundling distance (equilateral triangle for 3, square for 4).

            // Cached quantities:

            arma::Mat<Complex> ZPrim_; ///< Primative line impedance matrix.
            arma::Mat<Complex> ZPhase_; ///< Phase line impedance matrix.
            arma::Mat<Complex> YNode_; ///< Nodal admittance matrix.
    };
}

#endif // OVERHEAD_LINE_DOT_H
