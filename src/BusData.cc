#include "BusData.h"
#include "Output.h"

namespace SmartGridToolbox
{
   void Busses::addBus(int id, BusType type, double P, double V, double M,
                       double t)
   {
      busses_.emplace_back(id, type, P, V, M, t);
      if (type == BusType::PQ)
      {
         ++nPQ_;
      }
      else if (type == BusType::PV)
      {
         ++nPV_;
      }
      else if (type == BusType::SL)
      {
         ++nSL_;
      }
      else
      {
         error("Bad bus type.");
      }
   }
}
