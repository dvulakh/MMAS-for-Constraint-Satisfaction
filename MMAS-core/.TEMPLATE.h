
#ifndef UPROG_H
#define UPROG_H

#include "../MMAS-core/construction_graph_map.h"
#include "../MMAS-core/csp_construction_graph.h"

// The PROGSUFF class contains information about a problem
class PROGSUFF : public csp_graph
{
public:
	// Defines hard constraints
	// Return whether a new vertex may be added to a path
	// Ants never violate the hard constraints
	bool hard_constraint(path&, vertex&);
	// Defines cost incurred by adding new vertex to path
	// For CSPs, usually the number of soft constraint violations
	cost cost_of(path&, vertex&);
	// If you need any private members for your problem, add them here
	// See gcol_graph and sat_graph for examples of problems that use
	// private members to assess cost
};

/************** THIS IS THE FINAL DEFINITION THE ANTS WILL USE **************/
// This line creates a class that inherits the problem rules from the class you
// define above and the mechanism of pheromone storage we implement in the other
// source files.
class CLASS : public construction_graph_map, public PROGSUFF
{ public: CLASS(int nvar, int max_val, int plen, string cons); };

#endif