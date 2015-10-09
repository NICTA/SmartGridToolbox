//
//  Node.cpp
//  Cycle_Basis_PF
//
//  Created by Sumiran on 17/06/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#include "PowerTools++/Node.h"
#include "PowerTools++/Net.h"
#include "PowerTools++/Path.h"
#include <iostream>
#include <limits.h>

using namespace std;

Node::Node(){
    
}

Node::Node(string name, int id, double pl, double ql, double gs, double bs, double v_min, double v_max, double kvb,  int phase):Bus(name,pl,ql,gs,bs,v_min,v_max,kvb,phase), ID(id), explored(false), cycle(false), cg(false), fill_in(0), distance(INT_MAX), predecessor(NULL){}

Node::Node(string name, double pl, double ql, double gs, double bs, double v_min, double v_max, double kvb,  int phase):Node(name, -1, pl, ql, gs, bs, v_min, v_max, kvb, phase){}

Node::~Node(){
}

Node* Node::clone(){
    Node* copy = new Node();
    copy->ID = ID;
    return copy;
}


/* number of incident lines */
int Node::degree(){
    return (int)branches.size();
}


/*
 @brief Find and remove incident arc from list of branches
 @return 0 if a was found and removed, -1 oterwise 
 */
int Node::removeArc(Arc* a){
    vector<Arc*>::iterator it = branches.begin();
    while (it != branches.end()) {
        if((*it) == a){
            it = branches.erase(it);
            return 0;
        }
        it++;
    }
    return -1;
}

bool Node::is_connected(Node* n){
    for (auto a:branches) {
        if (n->ID==a->neighbour(this)->ID) {
            return true;
        }
    }
    for (auto a:n->branches) {
        if (ID==a->neighbour(n)->ID) {
            return true;
        }
    }
    return false;
}

bool Node::is_source(Node* n){
    for (auto a:branches) {
        if (ID == a->src->ID && n->ID==a->dest->ID) {
            return true;
        }
    }
    return false;
}

bool Node::is_sink(Node* n){
    for (auto a:n->branches) {
        if (n->ID == a->src->ID && ID==a->dest->ID) {
            return true;
        }
    }
    return false;
}


/*
 @brief Adds a to the list of incident arcs
 */
void Node::addArc(Arc* a){
    branches.push_back(a);
}

void Node::update_fill_in(Node* n){
    Node * nn = nullptr;
    for(auto a:branches){
        nn = a->neighbour(this);
        if (nn->ID==n->ID) {
            continue;
        }
        if (!n->is_connected(nn)) {
            fill_in++;
        }
        
    }
}
/*
 @brief Removes first incident arc from list of branches
 @return the next branch if any
 */
std::vector<Arc*>::iterator Node::removeFirstArc(){
    return branches.erase(branches.begin());
}



vector<Arc*> Node::get_out(){
    vector<Arc*> res;
    for (auto a:branches) {
        if (a->status==0) {
            continue;
        }
        if(a->src->ID==ID){
            res.push_back(a);
        }
    }
    return res;
}

vector<Arc*> Node::get_in(){
    vector<Arc*> res;
    for (auto a:branches) {
        if (a->status==0) {
            continue;
        }
        if(a->dest->ID==ID){
            res.push_back(a);
        }
    }
    return res;
}


//void Node::print(){
//    if(gen)
//        std::cout<< "Generator " << ID;
//    else
//        std::cout << "Bus " << ID;
//    if(gen){
//        std::cout << "; Pmin = " << Pmin;
//        std::cout << "; Pmax = " << Pmax;
//        std::cout << "; c1 = " << c1;
//        std::cout << "; c2 = " << c2;
//    }
//    else{
//        std::cout << "; Pl = " << Pl;
//        std::cout << "; Ql = " << Ql;
//    }
//    std::cout << "; Vmin = " << Vmin;
//    std::cout << "; Vmax = " << Vmax;
//    std::cout << "; Number of Branches = " << branches.size();
//    std::cout << "; Degree = " << degree()<< std::endl;
////    std::cout << "; Position = " << pos << std::endl;
//}

