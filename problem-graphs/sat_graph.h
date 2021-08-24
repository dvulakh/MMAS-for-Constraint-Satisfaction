
#ifndef SAT_H
#define SAT_H

#include "../MMAS-core/construction_graph_map.h"
#include "../MMAS-core/csp_construction_graph.h"

#include <fstream>
#include <set>

class sat_graph : public csp_graph
{
public:
    bool hard_constraint(path&, vertex&);
    cost cost_of(path&, vertex&);
    void sat_init(string conf);
    list<set<int>> clauses;
    int nclause;
};

class sat_graph_map : public construction_graph_map, public sat_graph
{ public: sat_graph_map(int nvar, int max_val, int plen, string conf); };

#endif