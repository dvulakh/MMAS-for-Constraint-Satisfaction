
#include "costas_graph.h"
#include <cmath>
#include <set>

typedef costas_graph costas;
typedef pheromone pher;
typedef vertex vert;

/*** GENERAL-LENGTH TAIL COSTAS GRAPH ***/

// Return cost of adding new vertex to path (number of new contradictions)
cost costas::cost_of(path& walk, vertex& nxt)
{
    /// Find all vectors already present
    cost c = 0;
    set<pair<int, int>> delta;
    int n = walk.first->size();
    auto it = walk.first->begin(), jt = it;
    for (int i = 0; i < n; i++, it++) {
        jt = walk.first->begin();
        for (int j = 0; j < i; j++, jt++)
            delta.insert({ i - j, *it - *jt });
    }
    /// Find all new vectors
    it = walk.first->begin();
    for (int i = 0; i < n; i++, it++)
        if (delta.find({ n - i, nxt - *it }) != delta.end())
            c++;
        else
            delta.insert({ n - i, nxt - *it });
    return c;
}

// Only hard constraint is no repetition
bool costas::hard_constraint(path& walk, vertex& nxt)
{
    for (vertex p : *walk.first)
        if (p == nxt)
            return false;
    return true;
}

/*** IMPLEMENTATIONS ***/
costas_graph_map::costas_graph_map(int nvar, int maxval, int plen, string cons) :
construction_graph_map(nvar, nvar, plen) { this->relative = true; }
costas_graph_map_mutex::costas_graph_map_mutex(int nvar) :
construction_graph_map_mutex(nvar, nvar) {}
costas_graph_arr::costas_graph_arr(int nvar, int tail) :
construction_graph_arr(nvar, nvar, tail) {}
