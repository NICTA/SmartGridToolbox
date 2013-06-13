#ifndef OUTPUT_DOT_H
#define OUTPUT_DOT_H

#include <cstdio>
#include <cstdarg>

namespace SmartGridToolbox
{
   void error(const char * fmt, ...);
   void warning(const char * fmt, ...);
   void message(const char * fmt, ...);
   // Debug is in header file so we can selectively debug particular parts
   // of the code more easily.
   static void debug(const char * fmt, ...)
   {
#ifdef DEBUG
      bool dbg = true;
#else
      bool dbg = false;
#endif
      if (dbg)
      {
         char buff[256];
         va_list args;
         va_start (args, fmt);
         vsnprintf (buff, 255, fmt, args);
         printf("DEBUG: %s\n", buff);
         va_end (args);
      }
   }
}

#endif // OUTPUT_DOT_H
