
#ifndef CONSTR_G_MAP_MUTEX
#define CONSTR_G_MAP_MUTEX

#include "construction_graph.h"
#include <unordered_map>

using namespace std;

typedef unordered_map<string, pheromone> mutex_graph_map;

class construction_graph_map_mutex : virtual public construction_graph
{
protected:
	virtual string path_str(path&, vertex&, int);
	virtual int edge_id(path&, vertex&, int);
	mutex_graph_map trails;
public:
	class map_iterator : public wrapped_iterator
	{
	public:
		map_iterator(mutex_graph_map::iterator, construction_graph_map_mutex*, bool);
		bool eq(wrapped_iterator*);
		construction_graph_map_mutex* g;
		pheromone& val();
		mutex_graph_map::iterator it;
		bool nulltau;
		void nxt();
	};
	construction_graph_map_mutex(int nvar, int max_val);
	void place_path(path&, vertex&, pheromone);
	void delta_path(path&, vertex&, pheromone);
	pair<pheromone, int> tau(path&, vertex&);
	pheromone nulltau;
	string log_str();
	shared_mutex mtx;
	iterator begin();
	iterator end();
	void init();
};

#endif
