#ifndef OBJECT_DOT_H
#define OBJECT_DOT_H

namespace MGSim
{
   class Simulation;

   /// Base class for simulation objects.
   class Object
   {
      public:
         /// Default constructor.
         Object(const Model & mod) : mod_(mod)
         {
            // Empty.
         }

         /// Copy constructor.
         Object(const Object & from) : mod_(from.mod_),
         {
            // Empty.
         }

         /// Destructor.
         virtual ~Object();

         /// Bring state up to time toTime.
         /** @param toTime */
         virtual void UpdateState(int toTime);

      private:

         const Model & mod_;
   } // class Object.
} // namespace MGSim.

#endif // OBJECT_DOT_H
