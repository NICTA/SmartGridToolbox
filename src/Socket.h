#ifndef SOCKET_DOT_H
#define SOCKET_DOT_H

#include "Common.h"
#include "Node.h"

namespace SmartGridToolbox
{
   class Socket
   {
      public:
         typedef std::vector<Node *>::iterator NodeIterator;
         typedef std::vector<Node *>::const_iterator NodeConstIterator;

         const std::string & getName const
         {
            return name_;
         }

      private:

         std::string name_;
         std::vector<Node *> nodes;
   }

   void Connect(Socket & socket1, Socket & socket2);
}

#endif // SOCKET_DOT_H
