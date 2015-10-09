//
//  Net.h
//  Cycle_Basis_PF
//
//  Created by Sumiran on 16/06/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#ifndef Cycle_Basis_PF_Net_h
#define Cycle_Basis_PF_Net_h

#include <map>
#include <math.h>
#include <PowerTools++/Arc.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <PowerTools++/Node.h>
#include <PowerTools++/Path.h>
#include <assert.h>

class Net{
public:
    

    double bMVA;
    
    string _name;
    
    /** Set of nodes */
    std::vector<Node*> nodes;
    
    /** Set of arcs */
    std::vector<Arc*> arcs;
    
    /** Set of generators */
    std::vector<Gen*> gens;
    
    /** Mapping the arcs to their source-destination */
    std::map<std::string, std::set<Arc*>*> lineID;
    
    /** Mapping the node id to its position in the vector, key = node id */
    std::map<std::string, Node*> nodeID;
    
    /** Vector of cycles forming a cycle basis */
    std::vector<Path*> cycle_basis;
    
    /** Horton subnetwork */
    Net* horton_net;

    /** Clone network */
    Net* _clone;
    
    /** Tree decomposition bags */
    std::vector<std::vector<Node*>*>* _bags;

    /** Compute the tree decomposition bags **/
    void get_tree_decomp_bags();
    
    double m_theta_ub;
    double m_theta_lb;
    
    /** structure for a new constraint */
    struct constraint{
        int cycle_index;
        /** it stors the gen node, the id of link tp next node, next node, id of link to prev node, prev node */
        int arr[5];
        double cnst;
    };
    
    /** vector of constraints bounding ratio of current magnitudes */
    std::vector<constraint> new_constraints;

    
    /* Constructors */
    
    Net();
    
    /* Destructors */
    
    ~Net();
    
    /** Erase Horton network */
    void clear_horton_net();
    
    /** Modifiers */
    
    void add_node(Node* n);
    
    /**  @brief Remove node and all incident arcs from the network
        @note Does not remove the incident arcs from the list of arcs in the network!
        @return the id of the node removed
     */
    std::string remove_end_node();
    
    bool add_arc(Arc* a);
    
    /** Returns true if the cycle in the bag is rotated in one direction only */
    bool rotation_bag(vector<Node*>* b);
    
    void remove_arc(Arc* a);

    /** Sort nodes in decreasing degree */
    void orderNodes(Net* net);
    
    /** Sort arcs in decreasing weight */
    void orderArcs(Net* net);
    
    void add_horton_nodes(Net* net);
    
    void add_horton_branches(Net* net);
    
    /* Accessors */
    
    Node* get_node(std::string id);

    /* Input Output */
    
    int readFile(std::string fname);
   
    void writeDAT(std::string name);
    
    /* Algorithms */
    
    /** Reset all distances to max value */
    void resetDistance();
    
    /** Computes and returns the shortest path between src and dest in net */
    Path* Dijkstra(Node* src, Node* dest, Net* net);
    
    /** Computes a spanning tree of minimal weight in horton's network, then adds the corresponding cycles to the original network by connecting to src
        @note Implements Kruskal’s greedy algorithm
     */
    void minimal_spanning_tree(Node* src, Net* net);

    /** Combines the src node with the path p to form a cycle */
    void combine(Node* src, Path* p);
    
    void Fast_Horton();
    
    void Fast_Horton(Net *net);
    
    /** adds decimal to an integer for precision */
    void precise(std::ofstream &myfile, float f);
    
    /** Cloning */
    Net* clone();
    
    /** finds arc parallel to the given arc and prints the self loop */
    int self_loops(std::ofstream &myfile, Arc* arc, int cycle_index);
    
    /** returns the arc formed by node ids n1 and n2 */
    Arc* get_arc(Node* n1, Node* n2);

    /** returns the arc formed by node ids n1 and n2 */
    Arc* get_arc(int n1, int n2);

    
    /** returns true if the arc formed by node ids n1 and n2 exists */
    bool has_arc(int n1, int n2);

    bool has_directed_arc(Node* n1, Node* n2);

    /** writes path p on myfile */
    void write(std::ofstream &myfile, Path* p, int cycle_index);
    
    /** returns true if an arc is already present between the given nodes */
    bool duplicate(int n1, int n2, int id1);
    
    /** identifies the generator bus in the cycle*/
    void s_flowIn_cycle(Path* p);
    
    /** checks neighbours of a gen bus if present in any cycle*/
    bool check_neighbour(Node* n);
    
    /** forms constraint for each cycle in the cycle basis */
    void Constraint();
    
    /** computes the constant for each gen node in each cycle */
    double compute_constant(Path* cycle);
    
    /** rearranges cycle according to gen node */
    Path* form_cycle(Node* n, Path* p);
    
    /** adds constraint to vector new_constraints */
    void addConstraint(int c, Path* cycle);
    
    /** sets the cycle member fo all Nodes to be false */
    void reset_nodeCycle();
    
    /** sets all nodes to unexplored */
    void reset_nodeExplored();
};
#endif
