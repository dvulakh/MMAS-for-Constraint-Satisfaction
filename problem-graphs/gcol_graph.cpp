
#include "gcol_graph.h"
#include <fstream>

typedef pheromone pher;
typedef gcol_graph gcl;
typedef vertex vert;

/*** GENERAL-LENGTH TAIL N-COLORING GRAPH ***/

// Initialize graph from file
void gcl::gcol_init(string cons)
{
	int e, u, v;
	ifstream fin(cons);
	adj = vector<set<int>>(nvar, set<int>());
	fin >> e;
	for (int i = 0; i < e; i++) {
		fin >> u >> v;
		adj[u - 1].insert(v);
		adj[v - 1].insert(u);
	}
}

// Return cost (number of new adjacent equally-colored vertices)
cost gcl::cost_of(path& walk, vertex& nxt)
{
	cost c = 0;
	auto it = walk.first->begin();
	auto jt = adj.begin();
	for (; it != walk.first->end(); it++, jt++)
		if (*it == nxt && jt->find(walk.first->size() + 1) != jt->end())
			c++;
	return c;
}

// No hard constraints
bool gcl::hard_constraint(path& walk, vertex& nxt) { return true; }

/*** IMPLEMENTATIONS ***/
gcol_graph_map::gcol_graph_map(int nvar, int max_val, int plen, string conf) :
construction_graph_map(nvar, max_val, plen)
{ gcol_init(conf); this->relative = true; this->circular = false; }
