
#include "construction_graph_map_mutex.h"

#define MOD 1000000007
#define INF 100000

typedef construction_graph::iterator cit;
typedef construction_graph_map_mutex graph;
typedef graph::map_iterator git;
typedef vertex vert;

/*** ARRAY ITERATOR ***/

// Constructor
git::map_iterator(mutex_graph_map::iterator it, graph* g, bool nulltau)
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
    bool r = nit->it == this->it && nit->g == g && nit->nulltau == nulltau;
    if (r && nit->it == g->trails.end())
        g->mtx.unlock();
    return r;
}

// Get next (increment iterator)
void git::nxt()
{
    if (nulltau)
        nulltau = false;
    else {
        bool inc = false;
        while (it != g->trails.end() && it->second <= g->nulltau)
            it = g->trails.erase(it), inc = true;
        if (!inc)
            it++;
    }
}

// Get value (dereference overload)
pheromone& git::val() { return nulltau ? g->nulltau : it->second; }

// Beginning and end iterators
cit graph::begin() { mtx.lock(); return iterator(new git(trails.begin(), this, true)); } // EXPLICITLY UNLOCK IF EXITING PREMATURELY
cit graph::end() { return iterator(new git(trails.end(), this, false)); }

/*** DEFAULT DEFINITIONS ***/

// Constructor
graph::construction_graph_map_mutex(int nvar, int max_val)
{
    this->max_val = max_val;
    this->nvar = nvar;
    init();
}

// Initialize pheromone map
void graph::init() { nulltau = INF; }

// Hash an edge: hashed by last tail elements of path (0 if none -- deprecated)
int graph::edge_id(path& walk, vertex& v, int tail)
{
    if (walk.first == NULL)
        return 0;
    long long d = v - 1, p = nvar + 2, i = 1;
    for (auto it = walk.first->rbegin(); it != walk.first->rend() && i < tail; it++, i++)
        d = (d + p * *it) % MOD, p = p * (nvar + 2) % MOD;
    return d;
}

// Make an edge into a string (backwards!)
string graph::path_str(path& walk, vertex& nxt, int tail)
{
    string s = to_string(nxt);
    for (auto it = walk.first->rbegin(); it != walk.first->rend() && tail - 1; it++, tail--)
        s += " " + to_string(*it);
    while (tail - 1)
        s += " 0", tail--;
    return s;
}

// Access/modify pheromone weights in graph using edge_id method
pair<pheromone, int> graph::tau(path& p, vert& v)
{
    if (!mtx.try_lock_shared())
        return pair<pheromone, int>({ nulltau, 0 });
    pair<pheromone, int> r;
    {
        auto it = trails.find(path_str(p, v, 2)); // FIX THIS
        r = { it != trails.end() ? it->second : nulltau, 2 };
    }
    mtx.unlock_shared();
    return r;
}
void graph::delta_path(path& walk, vert& nxt, pheromone dp) { place_path(walk, nxt, tau(walk, nxt).first + dp); }
void graph::place_path(path& walk, vert& nxt, pheromone p)
{
    mtx.lock();
    string hsh = to_string(nxt);
    for (auto it = walk.first->rbegin(); it != walk.first->rend(); it++) {
        hsh += " " + to_string(*it);
        if (p > nulltau && trails.find(hsh) == trails.end())
            trails.insert({ hsh, p });
        else if (p > nulltau)
            trails.find(hsh)->second = p;
        else
            trails.erase(hsh);
    }
    mtx.unlock();
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