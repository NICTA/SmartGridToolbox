// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

        PfBus(const std::string& id, BusType type, const Phases& phases, const arma::Col<Complex>& V,
                const arma::Mat<Complex>& YConst, const arma::Mat<Complex>& IConst, const arma::Mat<Complex>& SConst,
                const arma::Col<Complex>& SGen, double JGen);

        std::string id_; ///< Externally relevant id.
        BusType type_{BusType::BAD}; ///< Bus type.
        Phases phases_; ///< Bus phases.

        arma::Mat<Complex> V_; ///< Voltage, one per phase. Setpoint/warm start on input.
        arma::Mat<Complex> YConst_; ///< Constant admittance.
        arma::Mat<Complex> IConst_; ///< Constant current.
        arma::Mat<Complex> SConst_; ///< Constant power.
        arma::Mat<Complex> SGen_; ///< Generated power.

        // Regardless of numbers of poles, etc., the combined generation at a bus will have a certain angular
        // momentum for any given frequency of the network. Thus, we define an effective moment of inertia at the
        // bus by L = JGen omega_netw.
        double JGen_{0.0}; ///< Effective moment of inertia for all machines attached to bus.

        PfNodeVec nodeVec_; ///< Nodes, one per phase.
    };

    struct PfNode
    {
        public:
        PfBus* bus_{nullptr};
        std::size_t phaseIdx_{0};
        std::size_t idx_{0};

        PfNode(PfBus& bus, std::size_t phaseIdx);

        const Complex& V() const {return bus_->V_(phaseIdx_);}
        void setV(const Complex& V) {bus_->V_(phaseIdx_) = V;}

        const Complex& SGen() const {return bus_->SGen_(phaseIdx_);}
        void setSGen(const Complex& SGen) {bus_->SGen_(phaseIdx_) = SGen;}
    };

    struct PfBranch
    {
        PfBranch(const std::string& id0, const std::string& id1, const Phases& phases0, const Phases& phases1,
                const arma::Mat<Complex>& YConst);

        Array<std::string, 2> ids_; ///< Id of bus 0/1
        Array<Phases, 2> phases_; ///< phases of bus 0/1.
        arma::Mat<Complex> YConst_; ///< Bus admittance matrix.
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

        /// @name Lifecycle
        /// @{

        PowerFlowModel(size_t nBus, size_t nBranch);

        /// @}

        /// @name Bus, branch and node accessors.
        /// @{

        void addBus(const std::string& id, BusType type, const Phases& phases,
                const arma::Col<Complex>& V, const arma::Mat<Complex>& YConst, const arma::Mat<Complex>& IConst,
                const arma::Mat<Complex>& SConst, const arma::Col<Complex> SGen, double JGen);

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
                const Phases& phases0, const Phases& phases1, const arma::Mat<Complex>& YConst);

        const PfBranchVec& branchVec() const
        {
            return branchVec_;
        }
        PfBranchVec& branchVec()
        {
            return branchVec_;
        }

        const PfNodeVec& nodeVec() const
        {
            return nodeVec_;
        }
        PfNodeVec& nodeVec()
        {
            return nodeVec_;
        }

        /// @}

        /// @name Setup etc.
        /// @{

        void validate();
        void print();

        /// @}

        /// @name Branch flows.
        /// @{

        const arma::SpMat<Complex>& YConst() const
        {
            return YConst_;
        }
        arma::SpMat<Complex>& YConst()
        {
            return YConst_;
        }

        const arma::SpMat<Complex>& IConst() const
        {
            return IConst_;
        }
        arma::SpMat<Complex>& IConst()
        {
            return IConst_;
        }

        const arma::SpMat<Complex>& SConst() const
        {
            return SConst_;
        }
        arma::SpMat<Complex>& SConst()
        {
            return SConst_;
        }

        /// @}

        /// @name Vectors.
        /// SL node: V is a setpoint, SGen is a warm start.
        /// PQ node: V is a warm start, SGen is a setpoint.
        /// PV node: |V| is a setpoint, arg(V) is a warm start, Pg is a setpoint, Qg is a warm start.
        /// @{

        arma::Col<Complex> V() const; ///< Voltage.
        void setV(const arma::Col<Complex>& V) const; ///< Voltage.

        arma::Col<Complex> SGen() const;
        void setSGen(const arma::Col<Complex>& SGen) const;

        /// @}

        /// @name Count nodes of different types.
        /// Nodes are ordered as: SL first, then PQ, then PV.
        /// @{

        std::size_t nNode()
        {
            return nodeVec_.size();
        }
        std::size_t nPq()
        {
            return nPq_;
        }
        std::size_t nPv()
        {
            return nPv_;
        }
        std::size_t nPqPv()
        {
            return nPq_ + nPv_;
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

        arma::uword iPq(arma::uword i) const
        {
            return i;
        }
        arma::uword iPv(arma::uword i) const
        {
            return nPq_ + i;
        }
        arma::uword iSl(arma::uword i) const
        {
            return nPq_ + nPv_ + i;
        }
        arma::span selPq() const
        {
            return arma::span(iPq(0), iPq(nPq_ - 1));
        }
        arma::span selPv() const
        {
            return arma::span(iPv(0), iPv(nPv_ - 1));
        }
        arma::span selPqPv() const
        {
            return arma::span(iPq(0), iPv(nPv_ - 1));
        }
        arma::span selSl() const
        {
            return arma::span(iSl(0), iSl(nSl_ - 1));
        }
        arma::span selAll() const
        {
            return arma::span(iPq(0), iSl(nSl_ - 1));
        }

        /// @}

        private:

        /// @name vector of buses and branches.
        /// @{

        PfBusMap busMap_;
        PfBusVec busVec_;
        PfBranchVec branchVec_;
        PfNodeVec nodeVec_; // NOT owned by me - they are owned by their parent Buses.

        /// @}

        /// @name Array bounds.
        /// @{

        std::size_t nPq_; ///< Number of PQ nodes.
        std::size_t nPv_; ///< Number of PV nodes.
        std::size_t nSl_; ///< Number of SL nodes.

        /// @}

        /// @name Branch flows.
        /// @{

        arma::SpMat<Complex> YConst_; ///< bus admittance matrix for constant admittance.
        arma::SpMat<Complex> IConst_; ///< constant current elements.
        arma::SpMat<Complex> SConst_; ///< constant power elements.

        /// @}
    };
}

#endif // POWERFLOW_MODEL_DOT_H
