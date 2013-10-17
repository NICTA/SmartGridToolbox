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
         BusNR(const std::string & id, BusType type, Phases phases, const ublas::vector<Complex> & V,
               const ublas::vector<Complex> & Y, const ublas::vector<Complex> & I, const ublas::vector<Complex> & S);
         ~BusNR();

         std::string id_;                          ///< Externally relevant id. 
         BusType type_;                            ///< Bus type.
         Phases phases_;                           ///< Bus phases.
         ublas::vector<Complex> V_;                  ///< Voltage, one per phase.
         ublas::vector<Complex> Y_;                  ///< Constant admittance shunt, one per phase.
         ublas::vector<Complex> I_;                  ///< Constant current injection, one per phase.
         ublas::vector<Complex> S_;                  ///< Constant power injection, one per phase.

         typedef std::vector<NodeNR *> NodeVec;    ///< Nodes, one per phase.
         NodeVec nodes_;                           ///< Primary list of nodes.
   };

   class BranchNR
   {
      public:
         BranchNR(const std::string & id0, const std::string & id1, Phases phases0, Phases phases1, 
                  const ublas::matrix<Complex> & Y);

         int nPhase_;                  ///< Number of phases.
         Array<std::string, 2> ids_;   ///< Id of bus 0/1
         Array<Phases, 2> phases_;     ///< phases of bus 0/1.
         ublas::matrix<Complex> Y_;      ///< Bus admittance matrix.
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

         void addBus(const std::string & id, BusType type, Phases phases, const ublas::vector<Complex> & V,
                     const ublas::vector<Complex> & Y, const ublas::vector<Complex> & I, const ublas::vector<Complex> & S);

         const BusMap & busses()
         {
            return busses_;
         }

         void addBranch(const std::string & idBus0, const std::string & idBus1, Phases phases0, Phases phases1,
                        const ublas::matrix<Complex> & Y);

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
         /// @name ublas::vector of busses and branches.
         /// @{
         BusMap busses_;
         BranchVec branches_;

         // The following are NOT owned by me - they are owned by their parent Busses.
         NodeVec nodes_;
         NodeVec SLNodes_;
         NodeVec PQNodes_;

         /// @}

         /// @name Array bounds.
         /// @{
         int nSL_;                     ///< Number of slack nodes.
         int nPQ_;                     ///< Number of PQ nodes.
         int nPQPV_;                   ///< nPQ_ + nPV_.
         int nNode_;                   ///< Total number of nodes.
         int nVar_;                    ///< Total number of variables.
         /// @}

         /// @name ublas ranges into vectors/matrices.
         /// @{
         ublas::range rPQ_;              ///< Range of PQ nodes in list of all nodes.
         ublas::range rSL_;              ///< Range of SL nodes in list of all nodes.
         ublas::range rAll_;             ///< Range of all nodes in list of all nodes.
                                       /**< Needed for matrix_range. */
         ublas::range rx0_;              ///< Range of real voltage components in x_. 
         ublas::range rx1_;              ///< Range of imag voltage components in x_.
         /// @}

         ublas::vector<double> V0r_;     ///< Slack voltages real part - one per phase.
         ublas::vector<double> V0i_;     ///< Slack voltages imag part - one per phase.

         ublas::vector<double> PPQ_;     ///< Constant power injection of PQ nodes.
         ublas::vector<double> QPQ_;

         ublas::vector<double> IrPQ_;    ///< Constant current injection of PQ nodes.
         ublas::vector<double> IiPQ_;

         ublas::vector<double> Vr_;
         ublas::vector<double> Vi_;
         ublas::compressed_matrix<Complex> Y_;
         ublas::compressed_matrix<double> G_;
         ublas::compressed_matrix<double> B_;

         ublas::vector<double> x_;
         ublas::vector<double> f_;
         ublas::compressed_matrix<double> J_;
         ublas::compressed_matrix<double> JConst_; ///< The part of J that doesn't update at each iteration.
   };
}

#endif // POWER_FLOW_NR_DOT_H
