
#ifndef COSTAS_H
#define COSTAS_H

#include "../MMAS-core/construction_graph_map_mutex.h"
#include "../MMAS-core/construction_graph_arr.h"
#include "../MMAS-core/construction_graph_map.h"
#include "../MMAS-core/csp_construction_graph.h"

class costas_graph : public csp_graph
{
public:
	bool hard_constraint(path&, vertex&);
	cost cost_of(path&, vertex&);
};

class costas_graph_map_mutex : public construction_graph_map_mutex, public costas_graph
{ public: costas_graph_map_mutex(int nvar); };
class costas_graph_arr : public construction_graph_arr, public costas_graph
{ public: costas_graph_arr(int nvar, int tail); };
class costas_graph_map : public construction_graph_map, public costas_graph
{ public: costas_graph_map(int nvar, int maxval, int plen, string cons); };

#endif
