#ifndef MODEL_DOT_H
#define MODEL_DOT_H

namespace MGSim
{
   /// Simulation environment, containing global simulation data.
   /// Units: All units are internally stored in SI.
   class Model
   {
      public:
         /// Default constructor.
         Model() : dt_(1.0)
         {
            // Empty.
         }

         /// Destructor.
         ~Model();

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
   } // class Model.
} // namespace MGSim.

#endif // MODEL_DOT_H
