#include "Socket.h"
#include "Network.h"

void Connect(Socket & socket1, Socket & socket2, Network & network)
{
   for (Socket::NodeIterator it1 : socket1)
   {
      Socket::iterator it2 = find_if(socket2.begin(), socket2.end(),
            [&] (Socket::NodeIterator it2) -> bool 
            {return it2->Name() == it1->Name()});
      if (it2 != socket2.end())
      {
         // We found matching Nodes.
         Edge * edge = new Edge(
      }
   }
}
