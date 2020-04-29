
#include "costas_graph.h"
#include "ant_system.h"
#include "flags.h"

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <chrono>
#include <vector>

#define KILL 5000
#define NSET 16

using namespace std::chrono;
using namespace std;

typedef costas_graph_map costas;
typedef ant_system acs;

bool concurrent_trie_node::strict = false;

int main(int narg, char** args)
{

	// Graph and MMAS
	acs* as; costas* g;
	int nvar = -1; int plen = -1;
	
	// Flag definitions
	flag_reader fr;
	fr.add_flag("size", 'm', 1, [&](vector<string> v) { nvar = stoi(v[0]); });
	fr.add_flag("threads", 'n', 1, [&](vector<string> v) { as->ants = stoi(v[0]); });
	fr.add_flag("lambda", 'L', 1, [&](vector<string> v) { plen = stoi(v[0]); });
	fr.add_flag("ants", 'N', 1, [&](vector<string> v) { as->period = stoi(v[0]); });
	fr.add_flag("alpha", 'A', 1, [&](vector<string> v) { as->alpha = stod(v[0]); });
	fr.add_flag("beta", 'B', 1, [&](vector<string> v) { as->beta = stod(v[0]); });
	fr.add_flag("gamma", 'G', 1, [&](vector<string> v) { as->gamma = stod(v[0]); });
	fr.add_flag("rho", 'v', 1, [&](vector<string> v) { as->vap = stod(v[0]); });
	fr.add_flag("p0", 'p', 1, [&](vector<string> v) { as->p0 = stod(v[0]); });
	fr.add_flag("tmin", 0, 1, [&](vector<string> v) { as->tau_min = stod(v[0]); });
	fr.add_flag("tmax", 0, 1, [&](vector<string> v) { as->tau_max = stod(v[0]); });
	fr.add_flag("theta", 'Q', 1, [&](vector<string> v) { as->prune_buff = stoi(v[0]); });
	fr.add_flag("regicide", 't', 1, [&](vector<string> v) { as->regicide = stoi(v[0]); });
	fr.add_flag("restart", 'r', 0, [&](vector<string> v) { as->settings |= RE_START; });
	fr.add_flag("map", 'M', 0, [&](vector<string> v) { as->settings |= LOG_GRPH; });
	fr.add_flag("fout", 'r', 0, [&](vector<string> v) { as->fout_name = v[0]; });
	fr.add_flag("period", 'T', 1, [&](vector<string> v) { as->gl_period = stoi(v[0]); });
	fr.add_flag("data", 'D', 0, [&](vector<string> v) { as->settings |= LOG_DATA; });
	fr.add_flag("work", 'W', 0, [&](vector<string> v) { as->settings |= LOG_WORK; });
	fr.add_flag("trie", 0, 0, [&](vector<string> v) { as->settings |= USE_TRIE; });
	
	// Read input
	as = (new acs())->construct(NULL, 4, 10, 2, 25, 0.1, 0.01, 0, 2, 10, 5, "mmas.out", -1, 1, 0);
	if (fr.read_flags(narg, args) != FLAGS_INPUT_SUCCESS) {
		cout << "Invalid input" << endl;
		return 0;
	}
	/// Must have problem size
	if (nvar < 0) {
		cout << "Problem size: ";
		cin >> nvar;
	}
	g = new costas(nvar, plen);
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