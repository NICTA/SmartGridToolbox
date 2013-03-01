#ifndef SIM_OBJECT_DOT_H
#define SIM_OBJECT_DOT_H

namespace MGSim
{
   class Simulation;

   /// Base class for simulation objects.
   class SimObject
   {
      public:
         /// Default constructor.
         SimObject(const Model & mod) : mod_(mod)
         {
            // Empty.
         }

         /// Copy constructor.
         SimObject(const SimObject & from) : mod_(from.mod_),
         {
            // Empty.
         }

         /// Destructor.
         virtual ~SimObject();

         /// Bring state up to time toTime.
         /** @param toTime */
         virtual void UpdateState(int toTime);

      private:

         const Simulation & mod_;
   } // class SimObject.
} // namespace MGSim.

#endif // SIM_OBJECT_DOT_H
