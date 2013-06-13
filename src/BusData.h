#ifndef BUS_DATA_DOT_H
#define BUS_DATA_DOT_H

#include <vector>

namespace SmartGridToolbox
{
   enum class BusType
   {
      PQ,
      PV,
      SL
   };

   struct Bus
   {
      Bus(int id, BusType type, double P, double V, double M, double t) :
         id_(id), type_(type), P_(P), V_(V), M_(M), t_(t)
      {
         // Empty.
      }
      int id_;
      BusType type_;
      double P_;
      double V_;
      double M_;
      double t_;
   };

   class Busses
   {
      public:
         Busses() : nPQ_(0), nPV_(0), nSL_(0)
         {
            // Empty.
         }

         void addBus(int id, BusType type, double P, double V, double M,
                     double t);

         void validate();

         int getNPQ() {return nPQ_;}
         int getNPV() {return nPV_;}
         int getNSL() {return nSL_;}
         int getN() {return busses_.size();}
      
         const Bus & operator[](int idx) const {return busses_[idx];}
         Bus & operator[](int idx) {return busses_[idx];}

      private:
         int nPQ_;
         int nPV_;
         int nSL_;
         std::vector<Bus> busses_;
   };
}

#endif // BUS_DATA_DOT_H
