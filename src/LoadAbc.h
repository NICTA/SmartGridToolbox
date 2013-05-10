#ifndef LOAD_ABC_DOT_H
#define LOAD_ABC_DOT_H

namespace SmartGridToolbox

{
   class LoadAbc
   {
      public:
         virtual ~LoadAbc() {}
         virtual Complex getPower() = 0;
   };
}

#endif // LOAD_ABC_DOT_H
