#ifndef POWERFLOW_DOT_H
#define POWERFLOW_DOT_H

#include <SgtCore/Common.h>

#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace SmartGridToolbox
{
   enum class BusType
   {
      SL,
      PQ,
      PV,
      NA,
      BAD
   };

   const char* busType2Str(BusType type);
   inline std::ostream& operator<<(std::ostream& os, BusType t) {return os << busType2Str(t);}
   BusType str2BusType(const std::string& str);

   enum class Phase : unsigned int
   {
      BAL = 0x1,     // Balanced/one-phase.
      A   = 0x2,     // Three phase A.
      B   = 0x4,     // Three phase B.
      C   = 0x8,     // Three phase C.
      G   = 0x10,    // Ground.
      N   = 0x20,    // Neutral, to be used only when it is distinct from ground.
      SP  = 0x40,    // Split phase plus.
      SM  = 0x80,    // Split phase minus.
      SN  = 0x100,   // Split phase neutral.
      BAD = 0x200    // Not a phase.
   };

   const char* phase2Str(Phase phase);
   inline std::ostream& operator<<(std::ostream& os, Phase p) {return os << phase2Str(p);}
   Phase str2Phase(const std::string& str);
   const char* phaseDescr(Phase phase);

   class Phases
   {
      private:
         typedef std::map<Phase, unsigned int> IdxMap;
         typedef std::vector<Phase> PhaseVec;

      public:
         Phases() : mask_(0) {}
         Phases(unsigned int mask) : mask_(mask) {rebuild();}
         Phases(Phase phase) : mask_(static_cast<unsigned int>(phase)) {rebuild();}

         operator unsigned int() const {return mask_;}

         Phases& operator&=(const Phases& other);
         Phases& operator|=(const Phases& other);

         bool hasPhase(Phase phase) const
         {
            return (mask_& static_cast<unsigned int>(phase)) == static_cast<unsigned int>(phase);
         }
         bool isSubsetOf(const Phases& other) const {return (*this& other) == *this;}

         size_t size() const {return phaseVec_.size();}
         Phase operator[](int i) const {return phaseVec_[i];}
         int phaseIndex(Phase p) const {return hasPhase(p) ? idxMap_.at(p) : -1;}
         IdxMap::iterator begin() {return idxMap_.begin();}
         IdxMap::iterator end() {return idxMap_.end();}
         IdxMap::const_iterator begin() const {return idxMap_.begin();}
         IdxMap::const_iterator end() const {return idxMap_.end();}

         std::string toStr() const;

         friend bool operator==(const Phases& a, const Phases& b) {return a.mask_ == b.mask_;}
         friend Phases operator&(const Phases& a, const Phases& b) {return {a.mask_& b.mask_};}
         friend Phases operator|(const Phases& a, const Phases& b) {return {a.mask_ | b.mask_};}
         friend std::ostream& operator<<(std::ostream& os, const Phases& p) {return os << p.toStr();}
      private:
         void rebuild();

      private:
         unsigned int mask_;
         PhaseVec phaseVec_;
         IdxMap idxMap_;
   };

   inline Phases operator|(Phase a, Phase b) {return {static_cast<unsigned int>(a) | static_cast<unsigned int>(b)};}
   inline Phases operator&(Phase a, Phase b) {return {static_cast<unsigned int>(a) & static_cast<unsigned int>(b)};}
   
   struct PfNode;
   typedef std::vector<std::unique_ptr<PfNode>> PfNodeVec;

   // The following structures etc. are designed to present a general framework for solving power flow problems,
   // e.g. using Newton-Raphson or the swing equations.

   struct PfBus
   {
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

   class PowerFlowProblem
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

         void addBranch(const std::string& idBus0, const std::string& idBus1,
               const Phases& phases0, const Phases& phases1, const ublas::matrix<Complex>& Y);

         const PfBranchVec& branches() const
         {
            return branches_;
         }

         void reset();
         void validate();
         void printProblem();

      private:

         /// @name ublas::vector of busses and branches.
         /// @{
         
         PfBusMap busses_;
         PfBranchVec branches_;
         PfNodeVec nodes_; // NOT owned by me - they are owned by their parent Busses.
         
         /// @}
         
         /// @name Array bounds.
         /// @{
         
         unsigned int nSl_;   ///< Number of SL nodes.
         unsigned int nPq_;   ///< Number of PQ nodes.
         unsigned int nPv_;   ///< Number of PV nodes.
         
         /// @}

   };
}

#endif // POWERFLOW_DOT_H
