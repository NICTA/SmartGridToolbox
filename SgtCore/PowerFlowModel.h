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

#ifndef POWERFLOW_MODEL_DOT_H
#define POWERFLOW_MODEL_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace Sgt
{
    struct PfNode;

    // The following structures etc. are designed to present a general framework for solving power flow problems,
    // e.g. using Newton-Raphson or the swing equations.

    struct PfBus
    {
        typedef std::vector<std::unique_ptr<PfNode>> PfNodeVec;

        PfBus(const std::string& id, BusType type, const Phases& phases,
              const arma::Col<Complex>& YConst, const arma::Col<Complex>& IConst, const arma::Col<Complex>& SConst,
              double J, const arma::Col<Complex>& V, const arma::Col<Complex>& S);

        std::string id_; ///< Externally relevant id.
        BusType type_{BusType::BAD}; ///< Bus type.
        Phases phases_; ///< Bus phases.

        arma::Col<Complex> YConst_; ///< Constant admittance shunt, one per phase.
        arma::Col<Complex> IConst_; ///< Constant current injection, one per phase.
        arma::Col<Complex> SConst_; ///< Constant power injection, one per phase.

        // Regardless of numbers of poles, etc., the combined generation at a bus will have a certain angular
        // momentum for any given frequency of the network. Thus, we define an effective moment of inertia at the
        // bus by L = J omega_netw.
        double J_{0.0}; ///< Effective moment of inertia for all machines attached to bus.

        arma::Col<Complex> V_; ///< Voltage, one per phase. Setpoint/warm start on input.
        arma::Col<Complex> S_; ///< Total power injection, one per phase. Setpt/wm start on input.

        PfNodeVec nodes_; ///< Nodes, one per phase.
    };

    struct PfNode
    {
        PfNode(PfBus& bus, std::size_t phaseIdx);

        PfBus* bus_{nullptr};

        std::size_t phaseIdx_{0};

        Complex YConst_{0.0, 0.0};
        Complex IConst_{0.0, 0.0};
        Complex SConst_{0.0, 0.0};

        Complex V_{0.0, 0.0};
        Complex S_{0.0, 0.0};

        std::size_t idx_{0};
    };

    struct PfBranch
    {
        PfBranch(const std::string& id0, const std::string& id1, const Phases& phases0, const Phases& phases1,
                 const arma::Mat<Complex>& Y);

        std::size_t nPhase_{0}; ///< Number of phases.
        Array<std::string, 2> ids_; ///< Id of bus 0/1
        Array<Phases, 2> phases_; ///< phases of bus 0/1.
        arma::Mat<Complex> Y_; ///< Bus admittance matrix.
    };

    /// @brief A mathematical model of a network, suitable for solving the AC power flow problem.
    /// @ingroup PowerFlowCore
    class PowerFlowModel
    {
        public:
            typedef std::map<std::string, std::unique_ptr<PfBus>> PfBusMap;
            typedef std::vector<PfBus*> PfBusVec;
            typedef std::vector<std::unique_ptr<PfBranch>> PfBranchVec;
            typedef std::vector<PfNode*> PfNodeVec;

        public:

        /// @name Bus, branch and node accessors.
        /// @{

            void addBus(const std::string& id, BusType type, const Phases& phases,
                        const arma::Col<Complex>& YConst, const arma::Col<Complex>& IConst,
                        const arma::Col<Complex>& SConst, double J, const arma::Col<Complex>& V,
                        const arma::Col<Complex>& S);

            const PfBusMap& busMap() const
            {
                return busMap_;
            }
            PfBusMap& busMap()
            {
                return busMap_;
            }

            const PfBusVec& busVec() const
            {
                return busVec_;
            }
            PfBusVec& busVec()
            {
                return busVec_;
            }

            void addBranch(const std::string& idBus0, const std::string& idBus1,
                           const Phases& phases0, const Phases& phases1, const arma::Mat<Complex>& Y);

            const PfBranchVec& branches() const
            {
                return branches_;
            }
            PfBranchVec& branches()
            {
                return branches_;
            }

            const PfNodeVec& nodes() const
            {
                return nodes_;
            }
            PfNodeVec& nodes()
            {
                return nodes_;
            }

        /// @}

        /// @name Setup etc.
        /// @{

            void reset();
            void validate();
            void print();

        /// @}

        /// @name Y matrix.
        /// This absorbs any constant admittance ZIP components.
        /// @{

            const arma::SpMat<Complex>& Y() const
            {
                return Y_;
            }
            arma::SpMat<Complex>& Y()
            {
                return Y_;
            }

        /// @}

        /// @name Vectors.
        /// SL node: V is a setpoint, S is a warm start.
        /// PQ node: V is a warm start, S is a setpoint.
        /// PV node: |V| is a setpoint, arg(V) is a warm start, P is a setpoint, Q is a warm start.
        /// Note that P includes both load and generation and thus may include constant power components of ZIPs.
        /// For example, for a slack bus with a constant S load, P represents the generated power minus the load draw.
        /// @{

            const arma::Col<Complex>& V() const ///< Voltage.
            {
                return V_;
            }
            arma::Col<Complex>& V() ///< Voltage.
            {
                return V_;
            }

            const arma::Col<Complex>& S() const ///< Generated power injection minus load draw.
            {
                return S_;
            }
            arma::Col<Complex>& S() ///< Generated power injection minus load draw.
            {
                return S_;
            }

            const arma::Col<Complex>& IConst() const ///< Constant current component of ZIP.
            {
                return IConst_;
            }
            arma::Col<Complex>& IConst() ///< Constant current component of ZIP.
            {
                return IConst_;
            }

        /// @}

        /// @name Count nodes of different types.
        /// Nodes are ordered as: SL first, then PQ, then PV.
        /// @{

            std::size_t nNode()
            {
                return nodes_.size();
            }
            std::size_t nPq()
            {
                return nPq_;
            }
            std::size_t nPv()
            {
                return nPv_;
            }
            std::size_t nSl()
            {
                return nSl_;
            }

        /// @}

        /// @name Ordering of variables etc.
        /// @{

            // Note: a more elegant general solution to ordering would be to use matrix_slice. But assigning into
            // a matrix slice of a compressed_matrix appears to destroy the sparsity. MatrixRange works, but does not
            // present a general solution to ordering. Thus, when assigning into a compressed_matrix, we need to work
            // element by element, using an indexing scheme.

            arma::uword iSl(arma::uword i) const
            {
                return i;
            }
            arma::uword iPq(arma::uword i) const
            {
                return nSl_ + i;
            }
            arma::uword iPv(arma::uword i) const
            {
                return nSl_ + nPq_ + i;
            }

            arma::span selSlFromAll() const
            {
                return arma::span(0, arma::uword(nSl_ - 1));
            }
            arma::span selPqFromAll() const
            {
                return arma::span(arma::uword(nSl_), arma::uword(nSl_ + nPq_ - 1));
            }
            arma::span selPvFromAll() const
            {
                return arma::span(arma::uword(nSl_ + nPq_), arma::uword(nSl_ + nPq_ + nPv_ - 1));
            }
            arma::span selPqPvFromAll() const
            {
                return arma::span(arma::uword(nSl_), arma::uword(nSl_ + nPq_ + nPv_ - 1));
            }
            arma::span selAllFromAll() const
            {
                return arma::span(0, arma::uword(nSl_ + nPq_ + nPv_ - 1));
            }

        /// @}

        private:

        /// @name vector of busses and branches.
        /// @{

            PfBusMap busMap_;
            PfBusVec busVec_;
            PfBranchVec branches_;
            PfNodeVec nodes_; // NOT owned by me - they are owned by their parent Busses.

        /// @}

        /// @name Array bounds.
        /// @{

            std::size_t nSl_; ///< Number of SL nodes.
            std::size_t nPq_; ///< Number of PQ nodes.
            std::size_t nPv_; ///< Number of PV nodes.

        /// @}

        /// @name Y matrix.
        /// @{

            arma::SpMat<Complex> Y_; ///< Y matrix.

        /// @}

        /// @name Vectors per node.
        /// @{

            arma::Col<Complex> V_; ///< Complex voltage.
            arma::Col<Complex> S_; ///< Complex power injection = S_zip + S_gen
            arma::Col<Complex> IConst_; ///< Complex current.

        /// @}
    };
}

#endif // POWERFLOW_MODEL_DOT_H
