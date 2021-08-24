
#include "ant_system.h"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <random>
#include <time.h>
#include <cmath>

#define STRIP [&]{ while(line_string.length() && line_string[0] == ' ') line_string.erase(0, 1); }()
#define FTOK strtok(line, " \t")
#define IFTOK stoi(string(FTOK))
#define DFTOK stod(string(FTOK))
#define TOK strtok(NULL, " \t")
#define IWRD stoi(string(word))
#define DWRD stod(string(word))
#define ITOK stoi(string(TOK))
#define DTOK stod(string(TOK))

#define INF 2000000000
#define LINE_LEN 1000

typedef list<vertex>::iterator lvi;
typedef concurrent_trie_node trie;
typedef ant_system acs;
typedef pheromone pher;
typedef vertex vert;

/*** INITIALIZATION ***/

// Ant System Constructor
acs::ant_system() { construct(NULL, 25, 50, 2, 20, 0.25, 0.1, 0.01, 1, 5, -1, "", -1, 1, 0); }
ant_system* acs::construct(graph* g, int ants, int period, double alpha, double beta, double gamma, double vap, double p0, pher tau_min, pher tau_max, int prune_buff, string fout, int regicide, int gl_period, long long settings)
{
    this->prune_buff = prune_buff;
    this->gl_period = gl_period;
    this->regicide = regicide;
    this->settings = settings;
    this->tau_max = tau_max;
    this->tau_min = tau_min;
    this->fout_name = fout;
    this->period = period;
    this->gamma = gamma;
    this->alpha = alpha;
    this->beta = beta;
    this->ants = ants;
    this->vap = vap;
    this->p0 = p0;
    this->g = g;
    return this;
}

// Trail Initialization
void acs::init()
{
    /// Start timer
    start_time = chrono::high_resolution_clock::now();
    /// Seed random
    srand(time(NULL));
    /// Create work log files
    work_logs = new FILE*[ants + 1];
    data_logs = new FILE*[ants + 1];
    if (IS(LOG_WORK))
        for (int i = 0; i <= ants; i++)
            work_logs[i] = fopen(("./logs/pacs.work." + to_string(i) + ".log").c_str(), "w");
    if (IS(LOG_DATA))
        for (int i = 0; i <= ants; i++)
            data_logs[i] = fopen(("./logs/pacs.data." + to_string(i) + ".log").c_str(), "w");
    LPRINT(0, "Initializing ant system...");
    /// Fill trails
    allbest = list<path>(1, { NULL, INF });
    locbest = list<path>(1, { NULL, INF });
    bestval = INF, locval = INF;
    for (git it = g->begin(); it != g->end(); it++)
        *it = tau_max;
    /// Restart
    if (IS(RE_START))
        restart_from_file();
    /// Create map log file
    if (IS(LOG_GRPH))
        fout = fopen(fout_name.c_str(), IS(RE_START) ? "a" : "w");
    /// Initialize path trie
    tr = new trie(g->nvar, g->nvar);
    /// Initialize counters
    done = 0, iter = 0;
}

// Restart from old log file
void acs::restart_from_file()
{
    /// Get last block
    LPRINT(0, "Restarting...\n\tGetting last block of log...");
    system(("tac " + fout_name + " | sed '/\t/q' | tac > " + TEMP_RESTART_BUFFER).c_str());
    ifstream resin(TEMP_RESTART_BUFFER);
    /// Read solutions
    allbest.clear();
    char line[LINE_LEN], *word;
    resin.getline(line, LINE_LEN);
    int min_cost = IFTOK, num_line = ITOK;
    LPRINTF(0, "\tReading best solutions (%d)...", min_cost);
    bestval = min_cost;
    for (int i = 0; i < num_line; i++) {
        resin.getline(line, 1000);
        path p({new list<vertex>(), min_cost});
        word = FTOK;
        while (word != NULL)
            p.first->push_back(IWRD), word = TOK;
        allbest.push_back(p);
    }
    /// Start reading map
    LPRINT(0, "\tReading map...");
    resin.getline(line, LINE_LEN);
    num_line = IFTOK;
    /// Null pheromone
    pheromone tau = DTOK;
    for (git it = g->begin(); it != g->end(); it++)
        *it = tau;
    /// Individual entries
    string line_string;
    for (int i = 0; i < num_line; i++) {
        resin >> tau;
        getline(resin, line_string), STRIP;
        g->internal_set(line_string, tau);
    }
    LPRINT(0, "\tDone.");
}

/*** DESTRUCTOR ***/
acs::~ant_system()
{
    /// Halt processes
    LPRINT(0, "Halting processes...");
    halt();
    /// Delete trie
    LPRINT(0, "Deleting tree...");
    delete tr;
    /// Delete paths
    LPRINT(0, "Deleting paths...");
    erase(allbest);
    if (bestval < locval)
        erase(locbest);
    /// Close files
    LPRINT(0, "Deleting files...");
    for (int i = 0; i <= ants; i++) {
        if (IS(LOG_WORK))
            fclose(work_logs[i]);
        if (IS(LOG_DATA))
            fclose(data_logs[i]);
    }
    delete[] work_logs;
    delete[] data_logs;
}

/*** SINGLE ANT ***/

// Return numerator of probability of selecting vertex v to complete path walk
pher acs::prob(path& walk, vertex& v)
{
    auto t = g->tau(walk, v);
    return pow(t.first, alpha) * pow(g->heta(walk, v), beta) * pow(t.second, gamma);
}

// Select a candidate to append to path based on pheromone distributions
vert acs::rand_next(path& walk, trie* t)
{
    /// Get all candidates
    path cand = g->cand(walk);
    /// Remove candidates that have been completely searched
    vert bkup = cand.first->front();
    for (lvi it = cand.first->begin(); it != cand.first->end(); it)
        if (t != NULL && t->ch[*it] != NULL && t->ch[*it].load()->dinf.load().done)
            it = cand.first->erase(it);
        else
            it++;
    if (cand.first->empty()) {
        delete cand.first;
        return bkup;
    }
    /// With probability p0, choose highest pheromone candidate
    if ((double)rand() / RAND_MAX < p0) {
        vert nxt = cand.first->front();
        pher nmx = g->tau(walk, nxt).first;
        for (vert v : *cand.first) {
            auto tau = g->tau(walk, v);
            if (tau.first > nmx)
                nxt = v, nmx = tau.first;
        }
        delete cand.first;
        return nxt;
    }
    /// Otherwise, use standard ACO probability distribution
    else {
        /// Generate probability distributions
        pher sig = 0, num, rng;
        list<pher> pr_num;
        for (vertex v : *cand.first)
            pr_num.push_back(prob(walk, v)), sig += pr_num.back();
        /// Select randomly
        num = 0, rng = (double)rand() / RAND_MAX;
        auto it = pr_num.begin();
        for (vertex v : *cand.first) {
            num += *it;
            if ((double)num / sig >= rng) {
                delete cand.first;
                return v;
            }
            it++;
        }
    }
    /// This should not happen
    return cand.first->front();
}

// Simulate walk of single ant through construction graph, return its path
path acs::run_ant()
{
    /// Set initial path empty -- construction graph implementations MUST process empty path as request for valid starting points
    path walk = { new list<vert>(), 0 };
    trie* t = tr;
    vert nxt;
    /// Continue walk until all variables assigned
    while (walk.first->size() < g->nvar) {
        nxt = rand_next(walk, t);
        walk.second += g->cost_of(walk, nxt);
        walk.first->push_back(nxt);
        t = t == NULL ? NULL : t->ch[nxt].load();
        if (prune_buff >= 0 && walk.second >= bestval + prune_buff)
            return walk;
    }
    return walk;
}

// Ant process
void acs::ant_process(int id)
{
    int run_count = 1;
    while (running) {
        /// Generate walk
        LPRINTF(id, "Generating Walk %d\t", run_count);
        path walk = run_ant();
        LPRINTF(id, "\tGenerated cost %d", walk.second);
        DPRINTF(id, "%d\t%d", nupdate.load(), walk.second);
        /// Insert path serially
        if (IS(USE_TRIE)) {
            insert_path(walk.first->begin(), walk.first->end(), tr, prune_buff >= 0 && walk.second >= bestval + prune_buff);
            LPRINT(id, "\tPath inserted\t");
        }
        /// Update local bests with walk
        if (walk.second <= locval) {
            if (IS(ANTSPAWN))
                SPAWN(L(acs, update_best), walk);
            else
                update_best(walk);
            LPRINT(id, "\tInserted to locals");
        }
        else
            delete walk.first;
        /// Increment path count
        run_count++;
        done++;
    }
}

/*** TRAIL UPDATE ***/

// Deposit pheromones for specific path
void acs::deposit(path& walk)
{
    path cp = { new list<vert>(*walk.first), walk.second };
    vert nxt;
    while (!cp.first->empty())
        nxt = cp.first->back(), cp.first->pop_back(), g->place_path(cp, nxt, min(g->tau(cp, nxt).first + (pheromone)1 / cp.second, tau_max));
    delete cp.first;
}

// Update pheromone weights of construction graph -- increase only global best paths and local best paths (if different)
void acs::update_trails()
{
    /// Evaporate all trails
    for (git it = g->begin(); it != g->end(); it++)
        *it = max(*it * (1 - vap), tau_min);
    /// Deposit pheromones on best trails
    for (path w : allbest)
        deposit(w);
    if (bestval != locval && locval != INF)
        for (path w : locbest)
            deposit(w);
}

/*** BEST UPDATE ***/

// Path erase utility
void acs::erase(list<path>& lp)
{
    for (path w : lp)
        delete w.first;
}

// Update best value list argument using mutex
void acs::update_best_list(path& walk, list<path>& l, INT& v)
{
    while (true) {
        if (walk.second > v) {
            delete walk.first;
            return;
        }
        bool del = true, lck = mtx.try_lock();
        if (!lck)
            continue;
        if (running) {
            del = false;
            if (walk.second == v)
                l.push_back(walk);
            else if (walk.second < v)
                v = walk.second, erase(l), l.clear(), l.push_back(walk);
            else
                del = true;
        }
        mtx.unlock();
        if (del)
            delete walk.first;
        return;
    }
}

// Update local and global best values as necessary
void acs::update_best(path walk) { update_best_list(walk, locbest, locval); }

/*** COLONY QUEEN ***/

// Queen process: spawn all ants, periodically update pheromones, return solution (when found)
path* acs::queen_process()
{
    /// Fix graph values
    for (git it = g->begin(); it != g->end(); it++)
        *it = min(max(*it, tau_min), tau_max);
    gl_iter = -gl_period;
    /// Spawn ants
    running = true;
    LPRINT(0, "Spawning ants...\t");
    for (int i = 0; i < ants; i++)
        JOINABLE(L(acs, ant_process), i + 1);
    /// Mainloop
    while (running) {
        /// Perform regicide check
        if (regicide >= 0 && TPLUS(start_time).count() > regicide) {
            cout << "regicide" << endl;
            running = false;
            continue;
        }
        /// Check if new iteration complete
        if (done / period > iter && locval < INF) {
            /// Update counter
            iter = done / period;
            LPRINTF(0, "Iteration %d\t\t", iter);
            /// Prepare to update pheromones
            mtx.lock();
            /// If local optima equal in cost to global optima, add to globals
            if (locval == bestval)
                for (path w : locbest)
                    allbest.push_back(w);
            /// If local optima lower in cost that global optima, overwrite globals
            if (locval < bestval)
                running = bestval = locval.load(), erase(allbest), allbest = list<path>(locbest), cout << bestval << endl;
            /// Log values
            LPRINTF(0, "\tBest value: %d\t", bestval.load());
            LPRINTF(0, "\tLocal value: %d\t", locval.load());
            /// Update pheromones
            if (running) {
                LPRINT(0, "\tStarting map update...");
                update_trails();
                LPRINT(0, "\tMap update complete.")
            }
            nupdate++;
            /// Erase locals ONLY IF not in globals
            if (locval != bestval)
                erase(locbest);
            /// Make new locals and release mutex
            locval = INF, locbest.clear(), locbest.push_back({ NULL, INF });
            mtx.unlock();
            /// Print pheromone trails
            if (IS(LOG_GRPH) && iter >= gl_iter + gl_period) {
                gl_iter = iter;
                fprintf(fout, "%d\t%d\n", bestval.load(), allbest.size());
                for (path w : allbest) {
                    for (int v : *w.first)
                        fprintf(fout, "%d ", v);
                    fprintf(fout, "\n");
                }
                fprintf(fout, g->log_str().c_str());
                fflush(fout);
            }
            LPRINT(0, "\tDone\t\t");
        }
        /// Otherwise, sleep
        else
            SLEEP(QUEEN_PAUSE);
    }
    /// Return solution
    return &allbest.front();
}

// Halt all processes for safe deletion
void acs::halt()
{
    /// Join ants
    JOIN
}
