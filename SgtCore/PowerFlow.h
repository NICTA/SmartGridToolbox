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

#ifndef POWERFLOW_DOT_H
#define POWERFLOW_DOT_H

#include <SgtCore/Common.h>

#include <ostream>
#include <string>
#include <vector>

namespace Sgt
{
    /// @ingroup PowerFlowCore
    enum class BusType
    {
        SL,
        PQ,
        PV,
        NA,
        BAD
    };

    std::string to_string(BusType type);
    template<> struct JsonConvert<BusType>
    {
        static json toJson(BusType type)
        {
            return to_string(type);
        }
    };
    inline std::ostream& operator<<(std::ostream& os, BusType t) {return os << to_string(t);}
    template<> BusType from_string<BusType>(const std::string& str);

    /// @ingroup PowerFlowCore
    enum class Phase : std::uint8_t // Max of 255
    {
        BAL,    // Balanced/one-phase.
        A,      // Three phase A.
        B,      // Three phase B.
        C,      // Three phase C.
        G,      // Ground.
        N,      // Neutral, to be used only when it is distinct from ground.
        AB,     // Delta AB.
        BC,     // Delta BC.
        CA,     // Delta CA.
        SY0,    // Symmetrical zero sequence.
        SY1,    // Symmetrical positive sequence.
        SY2,    // Symmetrical negative sequence.
        SPP,    // Split phase plus.
        SPM,    // Split phase minus.
        SPN,    // Split phase neutral.
        BAD     // Not a phase.
    };

    constexpr size_t gNPhase = static_cast<std::uint8_t>(Phase::BAD) + 1;

    std::string to_string(Phase p);
    template<> struct JsonConvert<Phase>
    {
        static json toJson(Phase p)
        {
            return to_string(p);
        }
    };
    inline std::ostream& operator<<(std::ostream& os, Phase p) {return os << to_string(p);}
    template<> Phase from_string<Phase>(const std::string& str);

    /// @brief A set of network phases, each specified by a Phase value. 
    /// @ingroup PowerFlowCore
    class Phases
    {
        public:
            static constexpr uint8_t noSuchPhase = UINT8_MAX;

        public:
            Phases()
            {
                buildIndex();
            }

            Phases(Phase p) : vec_({p})
            {
                buildIndex();
            }
            
            Phases(std::initializer_list<Phase> ps) : vec_(ps)
            {
                buildIndex();
            }
            
            Phases(const std::vector<Phase>& ps) : vec_(ps)
            {
                buildIndex();
            }

            std::size_t size() const {return vec_.size();}
            Phase operator[](std::size_t i) const {return vec_[i];}

            std::uint8_t index(Phase p) const
            {
                return index_[static_cast<std::uint8_t>(p)];
            }

            std::vector<Phase>::iterator begin() {return vec_.begin();}
            std::vector<Phase>::iterator end() {return vec_.end();}
            std::vector<Phase>::const_iterator begin() const {return vec_.begin();}
            std::vector<Phase>::const_iterator end() const {return vec_.end();}
            std::vector<Phase>::const_iterator cbegin() const {return vec_.cbegin();}
            std::vector<Phase>::const_iterator cend() const {return vec_.cend();}

            std::string to_string() const;
            json toJson() const
            {
                return vec_;
            }

            friend bool operator==(const Phases& a, const Phases& b) {return a.vec_ == b.vec_;}

        private:
            void buildIndex();

        private:
            std::vector<Phase> vec_;
            std::uint8_t index_[gNPhase];
    };
    inline std::ostream& operator<<(std::ostream& os, const Phases& ps) {return os << ps.to_string();}

    /// @brief Apply Carson's equations.
    /// @param nWire The number of wires.
    /// @param Dij Distance between wires on off diagonal, GMR of wires on diagonal. Units: m.
    /// @param resPerL resistance per unit length, in ohms per metre.
    /// @param L The line length, in m.
    /// @param freq The system frequency.
    /// @param rhoEarth The conductivity of the earth, ohm metres.
    /// @return Line impedance matrix Z, s.t. I_phase = Z Delta V_phase.
    /// @ingroup PowerFlowCore
    arma::Mat<Complex> carson(arma::uword nWire, const arma::Mat<double>& Dij, const arma::Col<double> resPerL,
                              double L, double freq, double rhoEarth);

    /// @brief Apply the kron reduction to line impedance matrix Z.
    /// @param Z The primitive nWire x nWire line impedance matrix
    /// @param nPhase The number of phases, not including nWire grounded neutral wires that will be eliminated
    /// @return The impedance nPhase x nPhase matrix ZPhase, having eliminated nWire grounded neutrals.
    /// @ingroup PowerFlowCore
    arma::Mat<Complex> kron(const arma::Mat<Complex>& Z, arma::uword nPhase);

    /// @brief Calculate the nodal admittance matrix YNode from the line impedance ZLine.
    /// @param ZLine The line impedance matrix.
    /// @return The nodal admittance matrix YNode.
    /// @ingroup PowerFlowCore
    arma::Mat<Complex> ZLine2YNode(const arma::Mat<Complex>& ZLine);
    
    /// @brief Calculate the GMR of a bundled conductor.
    ///
    /// @param n The number of single conductors.
    /// @param gmr1 The GMR of a single conductor.
    /// @param d The side length of a regular polygon on whose vertices the single conductors lie.
    /// @return The GMR of the bundle.
    /// @ingroup PowerFlowCore
    double bundleGmr(unsigned int n, double gmr1, double d);
    
    /// @brief To sequence components transform for vector.
    /// @param v A 3 x 1 vector, 0 = A, 1 = B, 2 = C.
    /// @return The transformed vector: 0 = 0, 1 = +, 2 = -.
    /// @ingroup PowerFlowCore
    ///
    /// v_123 = A^-1 v_abc and M_123 = A^-1 M_abc A.
    arma::Col<Complex> toSequence(const arma::Col<Complex>& v);
    
    /// @brief To sequence components transform for matrix.
    /// @param M A 3 x 3 matrix, 0 = A, 1 = B, 2 = C.
    /// @return The transformed matrix: 0 = 0, 1 = +, 2 = -.
    /// @ingroup PowerFlowCore
    ///
    /// v_123 = A^-1 v_abc and M_123 = A^-1 M_abc A.
    arma::Mat<Complex> toSequence(const arma::Mat<Complex>& M);

    /// @brief From sequence components transform for vector.
    /// @param v A 3 x 1 vector, 0 = 0, 1 = +, 2 = -.
    /// @return The transformed vector: 0 = A, 1 = B, 2 = C.
    /// @ingroup PowerFlowCore
    ///
    /// v_123 = A^-1 v_abc and M_123 = A^-1 M_abc A.
    arma::Col<Complex> fromSequence(const arma::Col<Complex>& v);
    
    /// @brief From sequence components transform for matrix.
    /// @param M A 3 x 3 matrix, 0 = 0, 1 = +, 2 = -.
    /// @return The transformed matrix: 0 = A, 1 = B, 2 = C.
    /// @ingroup PowerFlowCore
    ///
    /// v_123 = A^-1 v_abc and M_123 = A^-1 M_abc A.
    arma::Mat<Complex> fromSequence(const arma::Mat<Complex>& M);
    
    /// @brief To sequence components transform for vector.
    /// @param v A 3 x 1 vector, 0 = A, 1 = B, 2 = C.
    /// @return The transformed vector: 0 = 0, 1 = +, 2 = -.
    /// @ingroup PowerFlowCore
    ///
    /// IMPORTANT: as per the theory notes, we use a scaled sequence transformation that is unitary and therefore
    /// preserves scalars like total power. Let B = sqrt(3) A, where A is the usual sequence transformation
    /// matrix. Then v_123 = B^-1 v_abc. A positive sequence vector V+ = [V_A, V_A exp(-2 pi / 3), V_A exp(2 pi / 3)]
    /// then transforms to sqrt(3) [0, V_A, 0]; note the sqrt(3) factor.
    arma::Col<Complex> toScaledSequence(const arma::Col<Complex>& v);
    
    /// @brief To sequence components transform for matrix.
    /// @param M A 3 x 3 matrix, 0 = A, 1 = B, 2 = C.
    /// @return The transformed matrix: 0 = 0, 1 = +, 2 = -.
    /// @ingroup PowerFlowCore
    ///
    /// IMPORTANT: as per the theory notes, we use a scaled sequence transformation that is unitary and therefore
    /// preserves scalars like total power. Let B = sqrt(3) A, where A is the usual sequence transformation
    /// matrix. Then v_123 = B^-1 v_abc. A positive sequence vector V+ = [V_A, V_A exp(-2 pi / 3), V_A exp(2 pi / 3)]
    /// then transforms to sqrt(3) [0, V_A, 0]; note the sqrt(3) factor.
    arma::Mat<Complex> toScaledSequence(const arma::Mat<Complex>& M);

    /// @brief From sequence components transform for vector.
    /// @param v A 3 x 1 vector, 0 = 0, 1 = +, 2 = -.
    /// @return The transformed vector: 0 = A, 1 = B, 2 = C.
    /// @ingroup PowerFlowCore
    ///
    /// IMPORTANT: as per the theory notes, we use a scaled sequence transformation that is unitary and therefore
    /// preserves scalars like total power. Let B = sqrt(3) A, where A is the usual sequence transformation
    /// matrix. Then v_123 = B^-1 v_abc. A positive sequence vector V+ = [V_A, V_A exp(-2 pi / 3), V_A exp(2 pi / 3)]
    /// then transforms to sqrt(3) [0, V_A, 0]; note the sqrt(3) factor.
    arma::Col<Complex> fromScaledSequence(const arma::Col<Complex>& v);
    
    /// @brief From sequence components transform for matrix.
    /// @param M A 3 x 3 matrix, 0 = 0, 1 = +, 2 = -.
    /// @return The transformed matrix: 0 = A, 1 = B, 2 = C.
    /// @ingroup PowerFlowCore
    ///
    /// IMPORTANT: as per the theory notes, we use a scaled sequence transformation that is unitary and therefore
    /// preserves scalars like total power. Let B = sqrt(3) A, where A is the usual sequence transformation
    /// matrix. Then v_123 = B^-1 v_abc. A positive sequence vector V+ = [V_A, V_A exp(-2 pi / 3), V_A exp(2 pi / 3)]
    /// then transforms to sqrt(3) [0, V_A, 0]; note the sqrt(3) factor.
    arma::Mat<Complex> fromScaledSequence(const arma::Mat<Complex>& M);

    /// @brief approximate the phase impedance matrix from Z+ and Z0
    /// @param ZPlus Positive sequence impedance
    /// @param Z0 Zero sequence impedance
    /// @return The approximate phase impedance matrix.
    ///
    /// Uses the usual sequence components.
    /// Often, we only know the + and 0 sequence impedances, which can typically be derived by noting the phase to
    /// ground and three phase short circuit currents or powers at a node (e.g. at the HV side of a distribution
    /// substation. Assuming a symmetry between the phases (e.g. through transposition or  symmetric arrangement of
    /// wires) leads to this approximate result for the phase impedance matrix. See Kersting, Distribution System
    /// Modelling and Analysis, Section 6.3.
    arma::Mat<Complex> approxPhaseImpedanceMatrix(Complex ZPlus, Complex Z0);
}

#endif // POWERFLOW_DOT_H
