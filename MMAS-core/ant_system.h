
#ifndef ANT_SYS
#define ANT_SYS

#include "concurrent_trie_node.h"
#include "construction_graph.h"
#include "multithread.h"
#include <stdio.h>
#include <fstream>
#include <chrono>

#define IS(S) (settings & (S))
#define USE_TRIE 1LL << 0
#define LOG_WORK 1LL << 1
#define LOG_DATA 1LL << 2
#define LOG_GRPH 1LL << 3
#define ANTSPAWN 1LL << 4
#define RE_START 1LL << 5

#define TEMP_RESTART_BUFFER ".temporary_restart_buffer"
#define ACS_FPRINTF(f, s, v, ...) { if (v) { fprintf(f, (s + string("\t%f\n")).c_str(), __VA_ARGS__, TPLUS(this->start_time)); fflush(f); } }
#define ACS_FPRINT(f, s, v) { if (v) { fprintf(f, (s + string("\t%f\n")).c_str(), TPLUS(this->start_time)); fflush(f); } }
#define LPRINTF(i, s, ...) ACS_FPRINTF(this->work_logs[i], s, IS(LOG_WORK), __VA_ARGS__)
#define DPRINTF(i, s, ...) ACS_FPRINTF(this->data_logs[i], s, IS(LOG_DATA), __VA_ARGS__)
#define LPRINT(i, s) ACS_FPRINT(this->work_logs[i], s, IS(LOG_WORK))
#define DPRINT(i, s) ACS_FPRINT(this->data_logs[i], s, IS(LOG_DATA))
#define QUEEN_PAUSE 100

typedef construction_graph graph;
typedef graph::iterator git;

class ant_system
{
public:
    
    /// Ant system start time
    chrono::high_resolution_clock::time_point start_time;
    /// Ant behavior definition: pheromone strength, heta strength, pheromone length value, vaporization rate, chance of optimal selection
    double alpha, beta, gamma, vap, p0;
    /// Optimal paths
    list<path> allbest, locbest;
    /// Min and max pheromone values
    pheromone tau_min, tau_max;
    /// Concurrent path trie
    concurrent_trie_node* tr;
    /// Worker threads list
    list<thread*> threads;
    /// Values of optimal paths
    INT bestval, locval;
    /// Settings: whether trie, work logs, and data logs are used
    long long settings;
    /// Period of queen graph logging and current graph logging iteration number
    int gl_period, gl_iter;
    /// Period of queen iteration and current iteration number
    int period, iter;
    /// Log file arrays; one log file and one data file per worker
    FILE** work_logs;
    FILE** data_logs;
    /// Values more expensive than global optimum by at least prunebuff are pruned; prunebuff = -1 for no pruning
    int prune_buff;
    /// Current state of ant system; used to halt workers on completion
    BOOL running;
    /// Time before forced queen termination; -1 if no time limit
    int regicide;
    /// Number of completed queen cycles (for data collection purposes)
    INT nupdate;
    /// Graph log file (for visualization purposes)
    string fout_name;
    FILE* fout;
    /// Mutex for best local path update
    mutex mtx;
    /// Number of ants in colony
    int ants;
    /// Number of ant walks completed
    INT done;
    /// Construction graph
    graph* g;

    ant_system* construct(graph* g, int ants, int period, double alpha, double beta, double gamma, double vap, double p0, pheromone tau_min, pheromone tau_max, int prune_buff, string fout, int regicide, int gl_period, long long settings);
    void update_best_list(path&, list<path>&, INT&);
    vertex rand_next(path&, concurrent_trie_node*);
    pheromone prob(path&, vertex&);
    void restart_from_file();
    void erase(list<path>&);
    void update_best(path);
    path* queen_process();
    void ant_process(int);
    void update_trails();
    void deposit(path&);
    path run_ant();
    ~ant_system();
    ant_system();
    void init();
    void halt();

};

#endif
