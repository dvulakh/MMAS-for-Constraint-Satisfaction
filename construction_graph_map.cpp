
#include "construction_graph_map.h"
#include <iostream>

#define MOD(x) (((x) % MOD_VAL + MOD_VAL) % MOD_VAL)
#define MOD_VAL 1000000007LL
#define INF 100000

#define START_HASH(walk, nxt) vert reference = nxt; string hsh = (circular ? "" : to_string(walk.first->size())) + (relative ? "" : to_string(nxt));
#define STEP_HASH(walk, nxt) hsh += (relative && it == walk.first->rbegin() ? "" : " ") + to_string(*it + (relative ? -1 : 0) * reference);

typedef construction_graph::iterator cit;
typedef construction_graph_map graph;
typedef graph::map_iterator git;
typedef vertex vert;

/*** MAP ITERATOR ***/

// Constructor
git::map_iterator(const concurrent_graph_map::iterator& it, graph* g, bool nulltau)
{
	this->nulltau = nulltau;
	this->it = it;
	this->g = g;
}

// Check for equality (equality overload)
bool git::eq(wrapped_iterator* it)
{
	if (typeid(*it) != typeid(git))
		return false;
	git* nit = dynamic_cast<git*>(it);
	return nit->it == this->it && nit->g == g && nit->nulltau == nulltau;
}

// Get next (increment iterator)
void git::nxt()
{
	if (nulltau)
		nulltau = false;
	else {
		bool inc = false;
		/// Filter uninteresting pheromones
		while (it != g->trails.end() && it->second <= g->nulltau) {
			/// Store key
			string key = it->first;
			/// Release iterator
			it.release();
			/// Erase iterator and get next
			it = g->trails.erase_and_return(it->first).unpad();
			/// Iterator has been incremented
			inc = true;
		}
		if (!inc)
			it++;
	}
}

// Get value (dereference overload)
pheromone& git::val() { return nulltau ? g->nulltau : it->second; }

// Beginning and end iterators
cit graph::begin() { return iterator(new git(trails.begin(), this, true)); }
cit graph::end() { return iterator(new git(trails.end(), this, false)); }

/*** DEFAULT DEFINITIONS ***/

// Constructor
graph::construction_graph_map(int nvar, int max_val, int plen)
{
	this->trails = concurrent_graph_map(str_hash);
	this->max_val = max_val;
	this->nvar = nvar;
	this->plen = plen;
	init();
}

// Destructor
graph::~construction_graph_map() { this->trails.destroy_internals(); }

// Initialize pheromone map
void graph::init() { nulltau = INF; }

// Hash an edge: hashed by last tail elements of path (0 if none -- deprecated)
int graph::edge_id(path& walk, vertex& v, int tail)
{
	if (walk.first == NULL)
		return 0;
	long long d = v - 1, p = nvar + 2, i = 1;
	for (auto it = walk.first->rbegin(); it != walk.first->rend() && i < tail; it++, i++)
		d = MOD(d + p * *it), p = p * MOD(nvar + 2);
	return d;
}

// Make an edge into a string (backwards!)
string graph::path_str(path& walk, vertex& nxt, int tail)
{
	START_HASH(walk, nxt);
	for (auto it = walk.first->rbegin(); it != walk.first->rend() && tail - 1; it++, tail--)
		STEP_HASH(walk, nxt);
	while (tail - 1)
		hsh += " 0", tail--;
	return hsh;
}

// String hash
long long str_hash(const string& s)
{
	long long h = 0;
	for (int i = 0; i < s.length(); i++)
		h = MOD(11 * h + (s[i] == ' ' || s[i] == '-' ? 0 : s[i] - '0'));
	return h;
}

// Access/modify pheromone weights in graph using string hash method
pair<pheromone, int> graph::tau(path& walk, vert& nxt)
{
	/// Arbitrary-length paths
	if (plen < 1) {
		/// Start path
		pair<pheromone, int> p = { 0, 0 };
		START_HASH(walk, nxt);
		for (auto it = walk.first->rbegin(); it != walk.first->rend(); it++) {
			/// Check for existance
			auto mit = trails.find(hsh);
			if (mit == trails.end())
				break;
			else {
				p = { mit->second, p.second + 1 };
				mit.release();
			}
			/// Add new element
			STEP_HASH(walk, nxt);
		}
		/// Return longest path
		return p.second > 0 ? p : pair<pheromone, int>({ nulltau, 1 });
	}
	/// Constant-length paths
	concurrent_graph_map::iterator it = trails.find(path_str(walk, nxt, plen));
	pheromone r = it != trails.end() ? it->second : nulltau;
	it.release();
	return { r, 1 };
}
void graph::internal_set(string hsh, pheromone p)
{
	concurrent_graph_map::iterator mit = trails.find(hsh);
	if (p > nulltau && mit == trails.end())
		trails.insert({ hsh, p });
	else if (p > nulltau)
		mit->second = p, mit.release();
	else
		mit.release(), trails.erase(hsh);
}
void graph::delta_path(path& walk, vert& nxt, pheromone dp) { place_path(walk, nxt, tau(walk, nxt).first + dp); }
void graph::place_path(path& walk, vert& nxt, pheromone p)
{
	/// Arbitrary-length paths
	if (plen < 1) {
		START_HASH(walk, nxt);
		for (auto it = walk.first->rbegin(); it != walk.first->rend(); it++) {
			STEP_HASH(walk, nxt);
			internal_set(hsh, p);
		}
	}
	/// Constant-length paths
	else
		internal_set(path_str(walk, nxt, plen), p);
}

// Log graph
string graph::log_str()
{
	string s = "";
	s += to_string(trails.size()) + " " + to_string(nulltau) + "\n";
	for (auto it = trails.begin(); it != trails.end(); it++)
		s += to_string(it->second) + " " + it->first + "\n";
	return s;
}
