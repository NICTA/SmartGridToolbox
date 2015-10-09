//
//  Arc.h
//  Cycle_Basis_PF
//
//  Created by Sumiran on 18/06/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#ifndef Cycle_Basis_PF_Arc_h
#define Cycle_Basis_PF_Arc_h
#include <PowerTools++/Node.h>
#include <PowerTools++/Path.h>
#include <PowerTools++/Line.h>

class Arc : public Line{
public:
    Node* src;
    Node* dest;
    bool in_cycle;
    bool parallel;
    Path* horton_path;
    
    double weight;
    
    /** @brief Phase angle difference bounds */
    Bound   tbound;
    
    /* @brief Returns the neighbour of n if n is a node of the arc, null otherwise */
    Node* neighbour(Node* n);
    
    Arc(std::string name);
    
    ~Arc();
    
    Arc(Node* s, Node* d);
    
    Arc* clone();
    
    /* Connects the current arc to its source and destination, adding itself to the list of branches in these nodes */
    void connect();
    
    void print();
};

#endif
