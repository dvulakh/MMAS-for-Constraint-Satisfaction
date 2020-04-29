
#include "construction_graph.h"

typedef construction_graph graph;
typedef graph::iterator git;

/*** WRAPPED ITERATOR ***/
pheromone& graph::wrapped_iterator::val() { pheromone p = 0; return p; }
bool graph::wrapped_iterator::eq(wrapped_iterator*) { return true; }
void graph::wrapped_iterator::nxt() {}

/*** VISIBLE ITERATOR ***/
git::iterator(wrapped_iterator* wit) { this->wit = wit; }
git& git::operator++(int) { iterator& it = *this; wit->nxt(); return it; }
bool git::operator==(const iterator& it) { return wit->eq(it.wit); }
bool git::operator!=(const iterator& it) { return !(*this == it); }
pheromone& git::operator*() { return wit->val(); }
git::~iterator() { delete wit; }

/*** DEFAULT DEFINITIONS ***/
pair<pheromone, int> graph::tau(path&, vertex&) { return pair<pheromone, int>({ 0, 1 }); }
git graph::begin() { return iterator(new wrapped_iterator()); }
git graph::end() { return iterator(new wrapped_iterator()); }
void graph::place_path(path&, vertex&, pheromone) {}
void graph::delta_path(path&, vertex&, pheromone) {}
pheromone graph::heta(path&, vertex&) { return 0; }
pheromone* graph::direct_tau(int) { return NULL; }
cost graph::cost_of(path&, vertex&) { return 0; }
path graph::cand(path&) { return { NULL, 0 }; }
void graph::internal_set(string, pheromone) {}
string graph::log_str() { return ""; }
void graph::init() {}