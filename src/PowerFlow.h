#ifndef POWERFLOW_DOT_H
#define POWERFLOW_DOT_H

#include "Common.h"

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
      BAD
   };

   const char * busType2Str(BusType type);
   inline std::ostream & operator<<(std::ostream & os, BusType t) {return os << busType2Str(t);}
   BusType str2BusType(const std::string & str);

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

   const char * phase2Str(Phase phase);
   inline std::ostream & operator<<(std::ostream & os, Phase p) {return os << phase2Str(p);}
   Phase str2Phase(const std::string & str);
   const char * phaseDescr(Phase phase);

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


         Phases & operator&=(const Phases & other);
         Phases & operator|=(const Phases & other);

         bool hasPhase(Phase phase) const
         {
            return (mask_ & static_cast<unsigned int>(phase)) == static_cast<unsigned int>(phase);
         }
         bool isSubsetOf(const Phases & other) const {return (*this & other) == *this;}

         int size() const {return phaseVec_.size();}
         Phase operator[](int i) const {return phaseVec_[i];}
         int phaseIndex(Phase p) const {return hasPhase(p) ? idxMap_.at(p) : -1;}
         IdxMap::iterator begin() {return idxMap_.begin();}
         IdxMap::iterator end() {return idxMap_.end();}
         IdxMap::const_iterator begin() const {return idxMap_.begin();}
         IdxMap::const_iterator end() const {return idxMap_.end();}

         std::string toStr() const;

         friend bool operator==(const Phases & a, const Phases & b) {return a.mask_ == b.mask_;}
         friend Phases operator&(const Phases & a, const Phases & b) {return {a.mask_ & b.mask_};}
         friend Phases operator|(const Phases & a, const Phases & b) {return {a.mask_ | b.mask_};}
         friend std::ostream & operator<<(std::ostream & os, const Phases & p) {return os << p.toStr();}
      private:
         void rebuild();

      private:
         unsigned int mask_;
         PhaseVec phaseVec_;
         IdxMap idxMap_;
   };

   inline Phases operator|(Phase a, Phase b) {return {static_cast<unsigned int>(a) | static_cast<unsigned int>(b)};}
   inline Phases operator&(Phase a, Phase b) {return {static_cast<unsigned int>(a) & static_cast<unsigned int>(b)};}

   /// Complex impedance  
   const ublas::matrix<Complex> YLine1P(const Complex & y);
   const ublas::matrix<Complex> YSimpleLine(const ublas::vector<Complex> & y);
   ublas::matrix<Complex> YOverheadLine(ublas::vector<double> r, ublas::matrix<double> DMat, 
                                        double freq, double rho = 100.0);
}

#endif // POWERFLOW_DOT_H
