
// Includes MMAS solver and flag interpreter -- do not remove
#include "ant_system.h"
#include "flags.h"

// Includes problem definitions -- add your header here
#include "template_graph.h" // <-- you can directly modify the template
#include "costas_graph.h"
#include "gcol_graph.h"
#include "sat_graph.h"

// Some STL utilities for main()
#include <iostream>
#include <chrono>
#include <vector>

using namespace std::chrono;
using namespace std;

// Typedefs for more convenient referencing of solver and graphs
// You can add your graph implementation here
typedef costas_graph_map costas;
typedef gcol_graph_map gcol;
typedef sat_graph_map sat;
typedef ant_system acs;

// This typedef determines the problem that will be solved
typedef template_graph_map problem_to_solve; // <<< REPLACE 'TEMPLATE_GRAPH_MAP'
                                             //     WITH YOUR GRAPH CLASS

// Helps with trie performance, though using the trie is not recommended
bool concurrent_trie_node::strict = false;

int main(int narg, char** args)
{
	
	// Graph and MMAS
	acs* as; problem_to_solve* g;
	string cons = "cons.in";
	int nvar = -1; int plen = -1; int dom = -1;
	
	// Flag definitions
	flag_reader fr;
	fr.add_flag("size", 'l', 1, [&](vector<string> v) { nvar = (int)stod(v[0]); });
	fr.add_flag("domain", 'm', 1, [&](vector<string> v) { dom = (int)stod(v[0]); });
	fr.add_flag("cons", 'C', 1, [&](vector<string> v) { cons = v[0]; });
	fr.add_flag("threads", 'n', 1, [&](vector<string> v) { as->ants = (int)stod(v[0]); });
	fr.add_flag("lambda", 'L', 1, [&](vector<string> v) { plen = (int)stod(v[0]); });
	fr.add_flag("ants", 'N', 1, [&](vector<string> v) { as->period = (int)stod(v[0]); });
	fr.add_flag("alpha", 'A', 1, [&](vector<string> v) { as->alpha = stod(v[0]); });
	fr.add_flag("beta", 'B', 1, [&](vector<string> v) { as->beta = stod(v[0]); });
	fr.add_flag("gamma", 'G', 1, [&](vector<string> v) { as->gamma = stod(v[0]); });
	fr.add_flag("rho", 'v', 1, [&](vector<string> v) { as->vap = stod(v[0]); });
	fr.add_flag("p0", 'p', 1, [&](vector<string> v) { as->p0 = stod(v[0]); });
	fr.add_flag("tmin", 0, 1, [&](vector<string> v) { as->tau_min = stod(v[0]); });
	fr.add_flag("tmax", 0, 1, [&](vector<string> v) { as->tau_max = stod(v[0]); });
	fr.add_flag("theta", 'Q', 1, [&](vector<string> v) { as->prune_buff = (int)stod(v[0]); });
	fr.add_flag("regicide", 't', 1, [&](vector<string> v) { as->regicide = (int)stod(v[0]); });
	fr.add_flag("restart", 'r', 0, [&](vector<string> v) { as->settings |= RE_START; });
	fr.add_flag("map", 'M', 0, [&](vector<string> v) { as->settings |= LOG_GRPH; });
	fr.add_flag("fout", 'r', 0, [&](vector<string> v) { as->fout_name = v[0]; });
	fr.add_flag("period", 'T', 1, [&](vector<string> v) { as->gl_period = (int)stod(v[0]); });
	fr.add_flag("data", 'D', 0, [&](vector<string> v) { as->settings |= LOG_DATA; });
	fr.add_flag("work", 'W', 0, [&](vector<string> v) { as->settings |= LOG_WORK; });
	fr.add_flag("trie", 0, 0, [&](vector<string> v) { as->settings |= USE_TRIE; });
	
	// Read input
	as = (new acs())->construct(NULL, 4, 10, 2, 25, 0.1, 0.01, 0, 2, 10, 5, "mmas.out", -1, 1, 0);
	if (fr.read_flags(narg, args) != FLAGS_INPUT_SUCCESS) {
		cout << "Invalid input" << endl;
		return 0;
	}
	// Must have problem size
	if (dom < 0) {
		cout << "Problem size: ";
		cin >> nvar;
	}
	// Default l = m
	if (nvar < 0)
		nvar = dom;
	g = new problem_to_solve(nvar, dom, plen, cons);
	as->g = g;
	
	// Run program
	auto tm = high_resolution_clock::now();
	as->init();
	path* pth = as->queen_process();
	cout << "Solution: ";
	for (auto it = pth->first->begin(); it != pth->first->end(); it++)
		cout << *it << " ";
	cout << "\nTime: " << duration_cast<milliseconds>(high_resolution_clock::now() - tm).count() / 1000.0 << endl;
	return 0;
	
}

