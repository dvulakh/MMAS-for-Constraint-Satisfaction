
#ifndef CONSTR_G
#define CONSTR_G

#include "multithread.h"
#include <functional>
#include <string>
#include <list>

using namespace std;

#define MAX_HETA 1000000

typedef int cost;
typedef int vertex;
typedef pair<list<vertex>*, cost> path;
typedef double pheromone;

class construction_graph
{
protected:
    class wrapped_iterator
    {
    public:
        virtual bool eq(wrapped_iterator*);
        virtual pheromone& val();
        virtual void nxt();
    };
public:
    class iterator
    {
    private:
        wrapped_iterator* wit;
    public:
        bool operator==(const iterator& it);
        bool operator!=(const iterator& it);
        iterator(wrapped_iterator*);
        iterator& operator++(int);
        pheromone& operator*();
        virtual ~iterator();
    };
    virtual void place_path(path&, vertex&, pheromone);
    virtual void delta_path(path&, vertex&, pheromone);
    virtual pair<pheromone, int> tau(path&, vertex&);
    virtual pheromone heta(path&, vertex&);
    virtual cost cost_of(path&, vertex&);
    virtual void internal_set(string, pheromone);
    virtual pheromone* direct_tau(int);
    virtual iterator begin();
    virtual path cand(path&);
    virtual string log_str();
    virtual iterator end();
    virtual void init();
    int max_val;
    int nvar;
};

#endif