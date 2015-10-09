//
//  Node.h
//  Cycle_Basis_PF
//
//  Created by Sumiran on 17/06/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#ifndef Cycle_Basis_PF_Node_h
#define Cycle_Basis_PF_Node_h
#include <PowerTools++/Bus.h>
#include <vector>
#include <PowerTools++/Line.h>


class Path;
class Arc;
class Net;

class Node : public Bus {
public:
    /* marks the node if it has been explored or not */
    bool explored;
    /* true if node is in cycle */
    bool cycle;
    /* if node in cycle and power flows in the cycle through this node */
    bool cg;
    /* the number of edges needed to make the subgraph formed by adjacent nodes a clique */
    int fill_in;
    
    int ID;
    Node* predecessor;
    int distance;
    std::vector<Arc*> branches;
    
    Node();
    Node(std::string name, double pl, double ql, double gs, double bs, double v_min, double v_max, double kvb,  int phase);
    Node(std::string name, int id, double pl, double ql, double gs, double bs, double v_min, double v_max, double kvb,  int phase);
    ~Node();
    Node* clone();
    
    /* number of incident lines */
    int degree();
    
    /*
     @brief Adds a to the list of incident arcs
     */
    void addArc(Arc* a);
    
    void update_fill_in(Node* n);

    /*
     @brief Returns true if n is an adjacent node.
     */
    bool is_connected(Node* n);
    
    /*
     @brief Returns true if there is an arc (this,n)
     */
    bool is_source(Node* n);
    
    /*
     @brief Returns true if there is an arc (n,this)
     */
    bool is_sink(Node* n);
    /*
     @brief Find and remove incident arc from list of branches
     @return 0 if a was found and removed, -1 otherwise
     */
    int removeArc(Arc* a);
    /*
     @brief Returns the vector of outgoing active arcs
     */
    vector<Arc*> get_out();
    
    /*
     @brief Returns the vector of incoming active arcs
     */
    vector<Arc*> get_in();
    
    /*
     @brief Removes first incident arc from list of branches
     @return 0 if the list contains at least one arc, -1 otherwise
     */
    std::vector<Arc*>::iterator removeFirstArc();
    
//    void print();

};

#endif
