#include "Output.h"

namespace SmartGridToolbox
{
   void error(const char * fmt, ...)
   {
      char buff[256];
      va_list args;
      va_start (args, fmt);
      vsnprintf (buff, 255, fmt, args);
      printf("ERROR: %s\n", buff);
      va_end (args);
      throw(buff);
   }

   void warning(const char * fmt, ...)
   {
      char buff[256];
      va_list args;
      va_start (args, fmt);
      vsnprintf (buff, 255, fmt, args);
      printf("WARNING: %s\n", buff);
      va_end (args);
   }

   void message(const char * fmt, ...)
   {
      char buff[256];
      va_list args;
      va_start (args, fmt);
      vsnprintf (buff, 255, fmt, args);
      printf("MESSAGE: %s\n", buff);
      va_end (args);
   }
}
