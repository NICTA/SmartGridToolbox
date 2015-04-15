#ifndef POWERFLOW_DOT_H
#define POWERFLOW_DOT_H

#include <SgtCore/Common.h>

#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace Sgt
{
    enum class BusType
    {
        SL,
        PQ,
        PV,
        NA,
        BAD
    };

    std::string to_string(BusType type);
    inline std::ostream& operator<<(std::ostream& os, BusType t) {return os << to_string(t);}
    template<> BusType from_string<BusType>(const std::string& str);

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

    std::string to_string(Phase phase);
    inline std::ostream& operator<<(std::ostream& os, Phase p) {return os << to_string(p);}
    template<> Phase from_string<Phase>(const std::string& str);
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

            std::string to_string() const;

            friend bool operator==(const Phases& a, const Phases& b) {return a.mask_ == b.mask_;}
            friend Phases operator&(const Phases& a, const Phases& b) {return {a.mask_& b.mask_};}
            friend Phases operator|(const Phases& a, const Phases& b) {return {a.mask_ | b.mask_};}
            friend std::ostream& operator<<(std::ostream& os, const Phases& p) {return os << p.to_string();}
        private:
            void rebuild();

        private:
            unsigned int mask_;
            PhaseVec phaseVec_;
            IdxMap idxMap_;
    };

    inline Phases operator|(Phase a, Phase b) {return {static_cast<unsigned int>(a) | static_cast<unsigned int>(b)};}
    inline Phases operator&(Phase a, Phase b) {return {static_cast<unsigned int>(a) & static_cast<unsigned int>(b)};}

    /// @brief Apply Carson's equations.
    /// @param nWire The number of wires.
    /// @param Dij Distance between wires on off diagonal, GMR of wires on diagonal. Units: m.
    /// @param resPerL resistance per unit length, in ohms per metre.
    /// @param L The line length, in m.
    /// @param freq The system frequency.
    /// @param rhoEarth The conductivity of the earth, ohm metres.
    /// @return Line impedance matrix Z, s.t. I_phase = Z \Delta V_phase.
    arma::Mat<Complex> carson(int nWire, const arma::Mat<double>& Dij, const arma::Col<double> resPerL,
                              double L, double freq, double rhoEarth);

    /// @brief Apply the kron reduction to line impedance matrix Z.
    /// @param Z The primitive nWire x nWire line impedance matrix
    /// @param nPhase The number of phases, not including nWire grounded neutral wires that will be eliminated
    /// @return The impedance nPhase x nPhase matrix ZPhase, having eliminated nWire grounded neutrals.
    arma::Mat<Complex> kron(const arma::Mat<Complex>& Z, int nPhase);

    /// @brief Calculate the nodal admittance matrix YNode from the line impedance ZLine.
    /// @param ZLine The line impedance matrix.
    /// @return The nodal admittance matrix YNode.
    arma::Mat<Complex> ZLine2YNode(const arma::Mat<Complex>& ZLine);
}

#endif // POWERFLOW_DOT_H
