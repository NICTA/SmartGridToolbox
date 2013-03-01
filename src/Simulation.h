#ifndef SIMULTION_DOT_H
#define SIMULTION_DOT_H

namespace MGSim
{
   /// Simulation. 
   class Simulation
   {
      public:
         /// Default constructor.
         Simulation(const Model & mod) : mod_(&mod)
         {
            // Empty.
         }

         /// Destructor.
         ~Simulation();

         /// Environment accessor.
         const Model & getModel() 
         {
            return *mod_;
         }

         /// Timestep accessor.
         double GetDt() const
         {
            return dt_;
         }
         /// Timestep accessor.
         void SetDt(double dt)
         {
            dt_ = dt;
         }

      private:
         double dt_; // Timestep in seconds.
         const Model * mod_;
   };
}

#endif // SIMULTION_DOT_H
