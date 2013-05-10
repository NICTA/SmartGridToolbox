#ifndef LOAD_DOT_H
#define LOAD_DOT_H

#include "Component.h"

namespace SmartGridToolbox

{
   class Load : public Component
   {
      // Inherited, overridden functions: from Component.
      public:
         virtual ptime getValidUntil() const override {return getTime() + dt_;}

      private:
         virtual void initializeState(ptime t) override;
         virtual void updateState(ptime t0, ptime t1) override;

      // Inherited, overridden functions: from Load.
      public:
         virtual Complex getPower() override {return Complex(Ph_, 0.0);}
      public:
         virtual ~Load() {}
         virtual Complex getPower() = 0;
   };

   class LoadComponent : public Component, public Load {};
}

#endif // LOAD_DOT_H
