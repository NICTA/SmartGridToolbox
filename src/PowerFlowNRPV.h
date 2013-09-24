#ifndef POWER_FLOW_NR_DOT_H
#define POWER_FLOW_NR_DOT_H

#include "Common.h"
#include "PowerFlow.h"
#include <vector>
#include <map>

// Terminology:
// "Bus" and "Branch" refer to n-phase objects i.e. they can contain several phases.
// "Node" and "Link" refer to individual bus conductors and single phase lines.
// A three phase network involving busses and branches can always be decomposed into a single phase network
// involving nodes and links. Thus use of busses and branches is simply a convenience that lumps together nodes and
// links.

namespace SmartGridToolbox
{
   class NodeNR;

   class BusNR
   {
      public:
         BusNR(const std::string & id, BusType type, Phases phases, const UblasVector<Complex> & V,
               const UblasVector<Complex> & Y, const UblasVector<Complex> & I, const UblasVector<Complex> & S);
         ~BusNR();

         std::string id_;                          ///< Externally relevant id. 
         BusType type_;                            ///< Bus type.
         Phases phases_;                           ///< Bus phases.
         UblasVector<Complex> V_;                  ///< Voltage, one per phase.
         UblasVector<Complex> Y_;                  ///< Constant admittance shunt, one per phase.
         UblasVector<Complex> I_;                  ///< Constant current injection, one per phase.
         UblasVector<Complex> S_;                  ///< Constant power injection, one per phase.

         typedef std::vector<NodeNR *> NodeVec;    ///< Nodes, one per phase.
         NodeVec nodes_;                           ///< Primary list of nodes.
   };

   class BranchNR
   {
      public:
         BranchNR(const std::string & id0, const std::string & id1, Phases phases0, Phases phases1, 
                  const UblasMatrix<Complex> & Y);

         int nPhase_;                  ///< Number of phases.
         Array<std::string, 2> ids_;   ///< Id of bus 0/1
         Array<Phases, 2> phases_;     ///< phases of bus 0/1.
         UblasMatrix<Complex> Y_;      ///< Bus admittance matrix.
   };

   class NodeNR
   {
      public:
         NodeNR(BusNR & bus, int phaseIdx);

         BusNR * bus_;
         int phaseIdx_;

         Complex V_;
         Complex Y_;
         Complex I_;
         Complex S_;

         int idx_;
   };

   class PowerFlowNR
   {
      public:
         typedef std::map<std::string, BusNR *> BusMap;  ///< Key is id.
         typedef std::vector<BranchNR *> BranchVec;
         typedef std::vector<NodeNR *> NodeVec;

      public:
         ~PowerFlowNR();

         void addBus(const std::string & id, BusType type, Phases phases, const UblasVector<Complex> & V,
                     const UblasVector<Complex> & Y, const UblasVector<Complex> & I, const UblasVector<Complex> & S);

         const BusMap & busses()
         {
            return busses_;
         }

         void addBranch(const std::string & idBus0, const std::string & idBus1, Phases phases0, Phases phases1,
                        const UblasMatrix<Complex> & Y);

         const BranchVec & branches()
         {
            return branches_;
         }

         void reset();
         void validate();
         bool solve();
         bool printProblem();

      private:
         void initx();
         void updateNodeV();
         void updateF();
         void updateJ();

      private:
         /// @name UblasVector of busses and branches.
         /// @{
         BusMap busses_;
         BranchVec branches_;

         // The following are NOT owned by me - they are owned by their parent Busses.
         NodeVec nodes_;
         NodeVec PQNodes_;
         NodeVec PVNodes_;
         NodeVec SLNodes_;

         /// @}

         /// @name Array bounds.
         /// @{
         int nPQ_;                     ///< Number of PQ nodes.
         int nPV_;                     ///< Number of PV nodes.
         int nPQPV_;                   ///< NPQ_ + NPV_.
         int nSL_;                     ///< Number of slack nodes.
         int nNode_;                   ///< Total number of nodes.
         int nVar_;                    ///< Total number of variables.
         /// @}

         /// @name ublas ranges and slices into vectors/matrices.
         /// @{
         UblasRange rPQ_;              ///< Range of PQ nodes in list of all nodes.
         UblasRange rPV_;              ///< Range of PV nodes in list of all nodes.
         UblasRange rPQPV_;            ///< Range of PQ and PV nodes in list of all nodes.
         UblasRange rSL_;              ///< Range of SL nodes in list of all nodes.
         UblasRange rAll_;             ///< Range of all nodes in list of all nodes.
         UblasSlice slx0_;             ///< Slice indexing variable 0 for each bus. 
         UblasSlice slx1_;             ///< Slice indexing variable 1 for each bus. 
         /// @}

         // Constant / supplied quantities:
         UblasVector<double> PPQ_;     ///< Constant real power injection of PQ nodes.
         UblasVector<double> QPQ_;     ///< Constant reactive power injection of PQ nodes.

         UblasVector<double> PPV_;     ///< Constant real power injection of PV nodes.
         UblasVector<double> VPV_;     ///< Constant voltage magnitude of PV nodes.

         UblasVector<double> VSLr_;    ///< Slack voltages real part - one per phase.
         UblasVector<double> VSLi_;    ///< Slack voltages imag part - one per phase.

         UblasVector<double> IrPQPV_;  ///< Constant real current injection of PQ and PV nodes.
         UblasVector<double> IiPQPV_;  ///< Constant imag current injection of PQ and PV nodes.

         // Vectors of unknowns:
         UblasVector<double> VrPQ_;    ///< Unknown real voltage of PQ nodes.   
         UblasVector<double> ViPQ_;    ///< Unknown imag voltage of PQ nodes.   

         UblasVector<double> VrPV_;    ///< Unknown real voltage of PV nodes.   
         UblasVector<double> QPV_;     ///< Unknown reactive power of PV nodes.   

         // Y matrix:
         UblasCMatrix<Complex> Y_;     ///< Complex Y matrix.
         UblasCMatrix<double> G_;      ///< Real part of Y matrix.
         UblasCMatrix<double> B_;      ///< Imag part of Y matrix.

         // Mathematical problem:
         UblasVector<double> x_;       ///< Vector of unknowns / LHS.
         UblasVector<double> f_;       ///< Function.
         UblasCMatrix<double> J_;      ///< Jacobian matrix.
         UblasCMatrix<double> JConst_; ///< The part of J that doesn't update at each iteration.
   };
}

#endif // POWER_FLOW_NR_DOT_H
