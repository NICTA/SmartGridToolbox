#ifndef POWERFLOW_MODEL_DOT_H
#define POWERFLOW_MODEL_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace SmartGridToolbox
{
   struct PfNode;

   // The following structures etc. are designed to present a general framework for solving power flow problems,
   // e.g. using Newton-Raphson or the swing equations.

   struct PfBus
   {
      typedef std::vector<std::unique_ptr<PfNode>> PfNodeVec;

      PfBus(const std::string& id, BusType type, const Phases& phases,
            const ublas::vector<Complex>& YZip, const ublas::vector<Complex>& IZip, const ublas::vector<Complex>& SZip,
            double J, const ublas::vector<Complex>& V, const ublas::vector<Complex>& S);

      std::string id_;              ///< Externally relevant id.
      BusType type_;                ///< Bus type.
      Phases phases_;               ///< Bus phases.

      ublas::vector<Complex> YZip_; ///< Constant admittance shunt, one per phase.
      ublas::vector<Complex> IZip_; ///< Constant current injection, one per phase.
      ublas::vector<Complex> SZip_; ///< Constant power injection, one per phase.

      ublas::vector<Complex> V_;    ///< Voltage, one per phase. Setpoint/warm start on input.
      ublas::vector<Complex> S_;    ///< Total power injection, one per phase. Setpt/wm start on input.

      // Regardless of numbers of poles, etc., the combined generation at a bus will have a certain
      // angular momentum for any given frequency of the network. Thus, we define an effective moment of inertia at the
      // bus by L = J_eff omega_netw.
      double J_;                    ///< Effective moment of inertia.

      PfNodeVec nodes_;             ///< Nodes, one per phase.
   };

   struct PfNode
   {
      PfNode(PfBus& bus, int phaseIdx);

      PfBus* bus_;

      int phaseIdx_;
      
      Complex YZip_;
      Complex IZip_;
      Complex SZip_;

      Complex V_;
      Complex S_;

      int idx_;
   };

   struct PfBranch
   {
      PfBranch(const std::string& id0, const std::string& id1, const Phases& phases0, const Phases& phases1,
            const ublas::matrix<Complex>& Y);

      int nPhase_;                ///< Number of phases.
      Array<std::string, 2> ids_; ///< Id of bus 0/1
      Array<Phases, 2> phases_;   ///< phases of bus 0/1.
      ublas::matrix<Complex> Y_;  ///< Bus admittance matrix.
   };

   class PowerFlowModel
   {
      public:
         typedef std::map<std::string, std::unique_ptr<PfBus>> PfBusMap;
         typedef std::vector<std::unique_ptr<PfBranch>> PfBranchVec;
         typedef std::vector<PfNode*> PfNodeVec;

      public:

         /// @name Bus, branch and node accessors.
         /// @{
         
         void addBus(const std::string& id, BusType type, const Phases& phases,
               const ublas::vector<Complex>& YZip, const ublas::vector<Complex>& IZip,
               const ublas::vector<Complex>& SZip, const ublas::vector<Complex>& V, const ublas::vector<Complex>& S);

         const PfBusMap& busses() const
         {
            return busses_;
         }
         PfBusMap& busses()
         {
            return busses_;
         }

         void addBranch(const std::string& idBus0, const std::string& idBus1,
               const Phases& phases0, const Phases& phases1, const ublas::matrix<Complex>& Y);

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
         
         const ublas::compressed_matrix<Complex>& Y() const
         {
            return Y_;
         }
         ublas::compressed_matrix<Complex>& Y()
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
         
         const ublas::vector<Complex>& V() const   ///< Voltage.
         {
            return V_;
         }
         ublas::vector<Complex>& V()               ///< Voltage.
         {
            return V_;
         }
         
         const ublas::vector<Complex>& S() const   ///< Generated power injection minus load draw.
         {
            return S_;
         }
         ublas::vector<Complex>& S()               ///< Generated power injection minus load draw.
         {
            return S_;
         }
         
         const ublas::vector<Complex>& IZip() const  ///< Constant current component of ZIP.
         {
            return IZip_;
         }
         ublas::vector<Complex>& IZip()              ///< Constant current component of ZIP.
         {
            return IZip_;
         }
         
         /// @}

         /// @name Count nodes of different types.
         /// Nodes are ordered as: SL first, then PQ, then PV. 
         /// @{
         
         size_t nNode()
         {
            return nodes_.size();
         }
         size_t nPq()
         {
            return nPq_;
         }
         size_t nPv()
         {
            return nPv_;
         }
         size_t nSl()
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

         int iSl(int i) const 
         {
            return i;
         }
         int iPq(int i) const 
         {
            return nSl_ + i;
         }
         int iPv(int i) const 
         {
            return nSl_ + nPq_ + i;
         }

         ublas::range selSlFromAll() const 
         {
            return {0, nSl_};
         }
         ublas::range selPqFromAll() const 
         {
            return {nSl_, nSl_ + nPq_};
         }
         ublas::range selPvFromAll() const 
         {
            return {nSl_ + nPq_, nSl_ + nPq_ + nPv_};
         }
         ublas::range selPqPvFromAll() const 
         {
            return {nSl_, nSl_ + nPq_ + nPv_};
         }
         ublas::range selAllFromAll() const 
         {
            return {0, nSl_ + nPq_ + nPv_};
         }

         /// @}

      private:

         /// @name ublas::vector of busses and branches.
         /// @{
         
         PfBusMap busses_;
         PfBranchVec branches_;
         PfNodeVec nodes_; // NOT owned by me - they are owned by their parent Busses.
         
         /// @}
         
         /// @name Array bounds.
         /// @{
         
         size_t nSl_;   ///< Number of SL nodes.
         size_t nPq_;   ///< Number of PQ nodes.
         size_t nPv_;   ///< Number of PV nodes.
         
         /// @}

         /// @name Y matrix.
         /// @{
         
         ublas::compressed_matrix<Complex> Y_;  ///< Y matrix.

         /// @}

         /// @name Vectors per node.
         /// @{

         ublas::vector<Complex> V_;    ///< Complex voltage.
         ublas::vector<Complex> S_;    ///< Complex power injection = S_zip + S_gen
         ublas::vector<Complex> IZip_;   ///< Complex voltage.
         
         /// @}
   };
}

#endif // POWERFLOW_MODEL_DOT_H
