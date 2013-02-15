#ifndef SIM_ENVIRONMENT_DOT_H
#define SIM_ENVIRONMENT_DOT_H

namespace MGSim
{
   class SimEnvironment
   {
      public:
         SimEnvironment() :
            dt_(1.0),
         {
         }

         ~SimEnvironment();

         double GetDt() const;
         void SetDt(double Dt);

      private:
         double dt_
   }
}

#endif // OBJECT_DOT_H
