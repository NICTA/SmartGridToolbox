#include <SmartGridToolbox/SimpleBuilding.h>
#include <SmartGridToolbox/Bus.h>

static double propTbNorm(double dt, double Tb0, double Ts, double kb,
                         double kh, double Cb)
{
   // Solve dTb = a(Ts - T), with a defined below.
   double a = (kb + kh)/Cb;
   double Tb1 = Ts + exp(-a*dt)*(Tb0 - Ts); 
   return Tb1;
}

static double propTbMaxed(double dt, double Tb0, double dQg0, double dQg1,
      double Te0, double Te1, double kb, double dQh, double Cb)
{
   // Solve dTb = a*T + b*t + c, with a, b, c defined below.
   double Tb1;
   if (dt == 0)
   {
      Tb1 = Tb0;
   }
   else
   {
      double a = -kb/Cb;
      double a2 = a*a;
      double b = ((dQg1 - dQg0)/dt + kb*(Te1-Te0)/dt)/Cb;
      double c = (dQg0 + kb*Te0 + dQh)/Cb;
      double eadt = exp(a*dt);
      Tb1 = (b*(-a*dt + eadt - 1) + a*(c*(eadt - 1) + a*Tb0*eadt))/a2;
   }
   return Tb1;
}

namespace SmartGridToolbox
{
   void SimpleBuilding::initializeState()
   {
      Tb_ = TbInit_;
      Ph_ = TbInit_;
      setOperatingParams(time());
   }

   void SimpleBuilding::updateState(Time t0, Time t1)
   {
      double dt = t0 == posix_time::neg_infin ? 0 : dSeconds(t1 - t0);

      if (!isMaxed_)
      {
         // Normal operation with controllers.
         Tb_ = propTbNorm(dt, Tb_, Ts_, kb_, kh_, Cb_);
      }
      else
      {
         // Power is constrained by maximum.
         Tb_ = propTbMaxed(dt, Tb_, dQg_(t0), dQg_(t1), Te_(t0), Te_(t1), kb_, dQh_, Cb_);
      }
      setOperatingParams(t1);
   }

   void SimpleBuilding::setOperatingParams(Time t)
   {
      dQh_ = -dQg_(t) + kb_*(Ts_ - Te_(t)) 
           + kh_*(Ts_ - Tb_); // Heat ADDED.
      mode_ = dQh_ > 0 ? HvacMode::HEATING : HvacMode::COOLING;
      cop_ = mode_ == HvacMode::HEATING ? copHeat_ : copCool_;
      Ph_ = abs(dQh_)/cop_;
      if (Ph_ > PMax_)
      {
         Ph_ = PMax_;
         dQh_ = mode_ == HvacMode::HEATING ? Ph_*cop_ : -Ph_*cop_;
         isMaxed_ = true;
      }
      else
      {
         isMaxed_ = false;
      }
   }
}
