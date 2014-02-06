#ifndef BRANCH_DOT_H
#define BRANCH_DOT_H

#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/Component.h>
#include <SmartGridToolbox/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   class Bus;

   class Branch : public Component
   {
      /// @name My public member functions.
      /// @{
      public:
         Branch(const std::string & name, const Phases & phases0, const Phases & phases1);

         const Bus & bus0() const {return *bus0_;}
         void setBus0(Bus & bus0);

         const Bus & bus1() const {return *bus1_;}
         void setBus1(Bus & bus1);

         const Phases & phases0() const {return phases0_;}

         const Phases & phases1() const {return phases1_;}

         const ublas::matrix<Complex> & Y() const {return Y_;}
         void setY(const ublas::matrix<Complex> & Y);
         
         Event & changed() {return changed_;}
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         Bus* bus0_;                      ///< My bus 0.
         Bus* bus1_;                      ///< My bus 1.
         Phases phases0_;                 ///< Phases on bus 0.
         Phases phases1_;                 ///< Phases on bus 1.
         ublas::matrix<Complex> Y_;       ///< Complex value of elements in bus admittance matrix in NR solver.
         Event changed_;                  ///< Updates when a setpoint property of the branch changes.
      /// @}
      /// @}
   };
}

#endif // BRANCH_DOT_H
