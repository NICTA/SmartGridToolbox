#ifndef NETWORK_COMPONENT_DOT_H
#define NETWORK_COMPONENT_DOT_H

#include "MGSim.h"

namespace MGSim
{

   class Socket
   {
      public:
         typedef std::vector<Node>::const_iterator const_iterator;

         const std::string & GetName const
         {
            return name_;
         }

         const std::vector & GetName const
         {
            return name_;
         }

      private:

         std::string name_;
         std::vector<Node> nodes;
   }

   /// Interface class for hooking together objects in a power network.
   class NetworkComponent
   {
      public:
         virtual NodeIterator 

   };
}

#endif // NETWORK_COMPONENT_DOT_H
