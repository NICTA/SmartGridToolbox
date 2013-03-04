#include "Socket.h"

void Connect(Socket & socket1, Socket & socket2)
{
   for (Socket::iterator it1 : socket1)
   {
      Socket::iterator it2 = find_if(socket2.begin(), socket2.end(),
            [&] (Socket::iterator it2) -> bool 
            {return it2->GetName() == it1->GetName()});
      if (it2 != socket2.end())
      {
         // We found matching Nodes.
         it1
      }
   }
}
