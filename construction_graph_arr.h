
#ifndef CONSTR_G_ARR
#define CONSTR_G_ARR

#include "construction_graph.h"
#include <cmath>

using namespace std;

class construction_graph_arr : virtual public construction_graph
{
protected:
	virtual int edge_id(path&, vertex&);
	pheromone* trails;
public:
	class arr_iterator : public wrapped_iterator
	{
	public:
		arr_iterator(int, construction_graph_arr*);
		bool eq(wrapped_iterator*);
		construction_graph_arr* g;
		pheromone& val();
		void nxt();
		int i;
	};
	construction_graph_arr(int nvar, int max_val, int tail);
	void place_path(path&, vertex&, pheromone);
	void delta_path(path&, vertex&, pheromone);
	pair<pheromone, int> tau(path&, vertex&);
	string log_str();
	iterator begin();
	iterator end();
	int trail_len;
	void init();
	int tail;
};

#endif
