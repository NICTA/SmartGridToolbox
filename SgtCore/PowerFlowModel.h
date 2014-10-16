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
            const ublas::vector<Complex>& Ys, const ublas::vector<Complex>& Ic,
            const ublas::vector<Complex>& V, const ublas::vector<Complex>& S);

      std::string id_;            ///< Externally relevant id.
      BusType type_;              ///< Bus type.
      Phases phases_;             ///< Bus phases.

      ublas::vector<Complex> Ys_; ///< Constant admittance shunt, one per phase.
      ublas::vector<Complex> Ic_; ///< Constant current injection, one per phase.

      ublas::vector<Complex> V_;  ///< Voltage, one per phase. Setpoint/warm start on input.
      ublas::vector<Complex> S_;  ///< Total power injection, one per phase. Setpt/wm start on input.

      PfNodeVec nodes_;             ///< Nodes, one per phase.
   };

   struct PfNode
   {
      PfNode(PfBus& bus, int phaseIdx);

      PfBus* bus_;

      int phaseIdx_;
      
      Complex Ys_;
      Complex Ic_;

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
         void addBus(const std::string& id, BusType type, const Phases& phases, const ublas::vector<Complex>& V,
               const ublas::vector<Complex>& Y, const ublas::vector<Complex>& I, const ublas::vector<Complex>& S);

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

         // The following are for indexing nodes. Nodes are ordered as: SL first, then PQ, then PV. 
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

         void reset();
         void validate();
         void print();

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
   };
}

#endif // POWERFLOW_MODEL_DOT_H
