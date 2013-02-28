#ifndef SIM_DOT_H
#define SIM_DOT_H

namespace MGSim
{
   /// Simulation. 
   class Simulation
   {
      public:
         /// Default constructor.
         Simulation(const SimEnv & env) : env_(env)
         {
            // Empty.
         }

         /// Destructor.
         ~Simulation();

         /// Environment accessor.
         const SimEnv & getSimEnv() 
         {
            return env_;
         }

      private:
         const SimEnv & env_;
   } // class SimEnv.
} // namespace MGSim.

#endif // SIM_DOT_H
