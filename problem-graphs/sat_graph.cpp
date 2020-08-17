
#include "sat_graph.h"

typedef pheromone pher;
typedef sat_graph sat;
typedef vertex vert;

/*** GENERAL-LENGTH TAIL SAT GRAPH ***/

// Initialize clauses from file
void sat::sat_init(string conf)
{
	ifstream fin(conf);
	fin >> nclause;
	int nv, v;
	for (int i = 0; i < nclause; i++) {
		clauses.push_back(set<int>());
		fin >> nv;
		for (int j = 0; j < nv; j++)
			fin >> v, clauses.back().insert(v);
	}
}

// Return cost of adding new vertex to path
// (number of new contradictions -- only computed at end) (1 = FALSE, 2 = TRUE)
cost sat::cost_of(path& walk, vertex& nxt)
{
	bool csat;
	cost c = 0;
	int i, j; vertex v;
	auto it = clauses.begin();
	auto jt = walk.first->begin();
	if (walk.first->size() + 1 == nvar)
		for (i = 0; i < nclause; i++, it++) {
			csat = false;
			for (j = 1, jt = walk.first->begin(); j <= nvar; j++, jt++) {
				v = jt == walk.first->end() ? nxt : *jt;
				if ((v == 1 && it->find(-j) != it->end()) || (v == 2 && it->find(j) != it->end()))
					csat = true;
			}
			c += !csat;
		}
	return c;
}

// No hard constraints
bool sat::hard_constraint(path& walk, vertex& nxt) { return true; }

/*** IMPLEMENTATIONS ***/
sat_graph_map::sat_graph_map(int nvar, int maxval, int plen, string conf) :
construction_graph_map(nvar, 2, plen)
{ sat_init(conf); this->relative = this->circular = false; }
