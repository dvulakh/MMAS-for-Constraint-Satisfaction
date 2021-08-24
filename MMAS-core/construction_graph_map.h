
#ifndef CONSTR_G_MAP
#define CONSTR_G_MAP

#include "construction_graph.h"
#include "concurrent_map.h"

using namespace std;

typedef concurrent_map<string, pheromone> concurrent_graph_map;

class construction_graph_map : virtual public construction_graph
{
protected:
    virtual string path_str(path&, vertex&, int);
    virtual int edge_id(path&, vertex&, int);
    concurrent_graph_map trails;
public:
    class map_iterator : public wrapped_iterator
    {
    public:
        map_iterator(const concurrent_graph_map::iterator&, construction_graph_map*, bool);
        concurrent_graph_map::iterator it;
        bool eq(wrapped_iterator*);
        construction_graph_map* g;
        pheromone& val();
        bool nulltau;
        void nxt();
    };
    construction_graph_map(int nvar, int max_val, int plen);
    void place_path(path&, vertex&, pheromone);
    void delta_path(path&, vertex&, pheromone);
    pair<pheromone, int> tau(path&, vertex&);
    void internal_set(string, pheromone);
    ~construction_graph_map();
    pheromone nulltau;
    string log_str();
    iterator begin();
    iterator end();
    bool relative;
    bool circular;
    void init();
    int plen;
};

long long str_hash(const string&);

#endif
