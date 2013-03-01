#ifndef SIMULTION_DOT_H
#define SIMULTION_DOT_H

namespace MGSim
{
   /// Simulation. 
   class Simulation
   {
      public:
         /// Default constructor.
         Simulation(const Model & mod) : mod_(mod)
         {
            // Empty.
         }

         /// Destructor.
         ~Simulation();

         /// Environment accessor.
         const Model & getModel() 
         {
            return mod_;
         }

      private:
         const Model & mod_;
   } // class Model.
} // namespace MGSim.

#endif // SIMULTION_DOT_H
