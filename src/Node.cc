#include "Node.h"

void Connect(Node & node1, Node & node2) {
   Edge * edge = new Edge(&node1, &node2);
   node1.edges_.push_back(
}
