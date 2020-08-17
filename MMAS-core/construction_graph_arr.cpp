
#include "construction_graph_arr.h"

typedef construction_graph::iterator cit;
typedef construction_graph_arr graph;
typedef graph::arr_iterator git;
typedef vertex vert;

/*** ARRAY ITERATOR ***/

// Constructor
git::arr_iterator(int i, graph *g)
{
	this->g = g;
	this->i = i;
}

// Check for equality (equality overload)
bool git::eq(wrapped_iterator* it)
{
	if (typeid(*it) != typeid(git))
		return false;
	git* nit = dynamic_cast<git*>(it);
	bool r = nit->i == i && nit->g == g;
	return r;
}

// Get next (increment index)
void git::nxt() { this->i++; }

// Get value (dereference overload)
pheromone& git::val() { return g->trails[i]; }

// Beginning and end iterators
cit graph::end() { return iterator(new git(trail_len, this)); }
cit graph::begin() { return iterator(new git(0, this)); }

/*** DEFAULT DEFINITIONS ***/

// Constructor
graph::construction_graph_arr(int nvar, int max_val, int tail)
{
	this->max_val = max_val;
	this->nvar = nvar;
	this->tail = tail;
	init();
}

// Initialize pheromone matrix
void graph::init()
{
	trail_len = (max_val + 1) * pow(nvar + 2, tail);
	trails = new pheromone[trail_len];
}

// Hash an edge: hashed only by last this->tail elements of path (0 if none)
int graph::edge_id(path& walk, vertex& v)
{
	int d = v - 1, p = nvar + 2, i = 1;
	for (auto it = walk.first->rbegin(); it != walk.first->rend() && i < tail; it++, i++)
		d += *it * p, p *= (nvar + 2);
	return d;
}

// Access/modify pheromone weights in graph using edge_id method
pair<pheromone, int> graph::tau(path& p, vert& v)
{
	int i = edge_id(p, v);
	return pair<pheromone, int>({ i >= 0 ? trails[i] : 1, trail_len });
}
void graph::delta_path(path& walk, vert& nxt, pheromone dp) { trails[edge_id(walk, nxt)] += dp; }
void graph::place_path(path& walk, vert& nxt, pheromone p) { trails[edge_id(walk, nxt)] = p; }

// Log graph
string graph::log_str()
{
	string s = "";
	for (int i = 0; i <= nvar; i++) {
		for (int j = 1; j <= nvar; j++)
			s += to_string(trails[i * (nvar + 1) + j]) + " ";
		s += "\n";
	}
	return s + "\n";
}