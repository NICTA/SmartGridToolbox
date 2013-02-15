#ifndef OBJECT_DOT_H
#define OBJECT_DOT_H

namespace MGSim
{
   class Object
   {
      public:
         Object()
         {
            // Empty.
         }

         virtual ~Object() = 0;

         double T();

         bool update
   }

   inline Object::~Object()
   {
      // Empty.
   }
}

#endif // OBJECT_DOT_H
