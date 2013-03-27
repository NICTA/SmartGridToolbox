#ifndef SIMULTION_DOT_H
#define SIMULTION_DOT_H

#include "Common.h"

namespace SmartGridToolbox
{
   /// Simulation. 
   class Simulation
   {
      public:
         /// Default constructor.
         Simulation(Model & mod) : mod_(&mod)
         {
            // Empty.
         }

         /// Destructor.
         ~Simulation();

         /// Model accessor.
         const Model & GetModel() const
         {
            return *mod_;
         }
         /// Model accessor.
         Model & GetModel()
         {
            return const_cast<Model &>((const_cast<const Simulation *>(this))->
                  GetModel());
         }

         /// Advance timestep.
         int AdvanceTimestep() 
         {
            
         }


      private:
         Model * mod_;
         TimestampType it_; // Current step.
   };
}

#endif // SIMULTION_DOT_H
