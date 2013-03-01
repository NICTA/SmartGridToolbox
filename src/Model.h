#ifndef SIM_ENV_DOT_H
#define SIM_ENV_DOT_H

namespace MGSim
{
   /// Simulation environment, containing global simulation data.
   /// Units: All units are internally stored in SI.
   class SimEnv
   {
      public:
         /// Default constructor.
         SimEnv() : dt_(1.0)
         {
            // Empty.
         }

         /// Destructor.
         ~SimEnv();

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
   } // class SimEnv.
} // namespace MGSim.

#endif // SIM_ENV_DOT_H
