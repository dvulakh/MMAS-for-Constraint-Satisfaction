
#ifndef CSP_GRAPH
#define CSP_GRAPH

#include "construction_graph.h"

class csp_graph : virtual public construction_graph
{
public:
	virtual bool hard_constraint(path&, vertex&) { return true; }
	pheromone heta(path&, vertex&);
	path cand(path&);
};

#endif
