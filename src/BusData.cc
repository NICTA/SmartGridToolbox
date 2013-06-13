#include "BusData.h"
#include "Output.h"
#include <algorithm>

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

   void Busses::validate()
   {
      std::sort(busses_.begin(), busses_.end(), 
            [](const Bus & lhs, const Bus & rhs) -> bool 
            {return (lhs.type_ < rhs.type_) ||
                    ((lhs.type_ == rhs.type_)) && (lhs.id_ < rhs.id_);});
      for (const Bus & bus : busses_)
      {
         debug("Bus id = %d type = %d", bus.id_, bus.type_);
      }
      if (nSL_ != 1)
      {
         error("There must be exactly one slack bus.");
      }
   }
}
