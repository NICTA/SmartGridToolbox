#ifndef SOCKET_DOT_H
#define SOCKET_DOT_H

#include "SmartGridToolbox.h"
#include "Node.h"

namespace SmartGridToolbox
{
   class Socket
   {
      public:
         typedef std::vector<Node *>::iterator NodeIterator;
         typedef std::vector<Node *>::const_iterator NodeConstIterator;

         const std::string & GetName const
         {
            return name_;
         }

         const_iterator begin() const
         {
            return nodes.begin();
         }
         iterator begin()
         {
            return nodes.begin();
         }

         const_iterator end() const
         {
            return nodes.end();
         }
         iterator end()
         {
            return nodes.begin();
         }

      private:

         std::string name_;
         std::vector<Node *> nodes;
   }

   void Connect(Socket & socket1, Socket & socket2);
}

#endif // SOCKET_DOT_H
