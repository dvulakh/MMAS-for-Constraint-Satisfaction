
#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "construction_graph_map.h"
#include "csp_construction_graph.h"

// The template_graph class contains information about a problem
class template_graph : public csp_graph
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

/****** THIS IS THE NAME OF THE CLASS YOU SHOULD REFERENCE IN MMAS.CPP ******/
// You probably will not need to change this line (except possibly to rename
// template_* to something more meaningful, such as the problem it represents.
// This line creates a class that inherits the problem rules from the class you
// define above and the mechanism of pheromone storage we implement in the other
// source files.
class template_graph_map : public construction_graph_map, public template_graph { public: template_graph_map(int nvar, int max_val, int plen, string cons); };

#endif