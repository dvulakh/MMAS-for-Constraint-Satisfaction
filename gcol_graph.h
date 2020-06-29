
#ifndef GCOL_H
#define GCOL_H

#include "construction_graph_map.h"
#include "csp_construction_graph.h"

#include <fstream>
#include <vector>
#include <set>

class gcol_graph : public csp_graph
{
public:
	bool hard_constraint(path&, vertex&);
	cost cost_of(path&, vertex&);
	void gcol_init(string cons);
	vector<set<int>> adj;
	int ncolor;
};

class gcol_graph_map : public construction_graph_map, public gcol_graph { public: gcol_graph_map(int nvar, int max_val, int plen, string cons); };

#endif
