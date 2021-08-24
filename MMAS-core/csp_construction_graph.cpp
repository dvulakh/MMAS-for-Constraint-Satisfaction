
#include "csp_construction_graph.h"

typedef csp_graph graph;

// Heta heuristic value is the inverse of cost
pheromone graph::heta(path& walk, vertex& nxt)
{
    cost c = cost_of(walk, nxt);
    return c ? (pheromone)1 / c : MAX_HETA;
}

// Candidate values are those permitted by hard constraints
path graph::cand(path& walk)
{
    path cnd = { new list<vertex>(), 0 };
    for (int i = 1; i <= max_val; i++)
        if (hard_constraint(walk, i))
            cnd.first->push_back(i);
    return cnd;
}
