
#ifndef CON_TREE_NODE
#define CON_TREE_NODE

#include "construction_graph.h"
#include "multithread.h"
#include <algorithm>

struct done_info
{
	double comp;
	bool done;
	done_info(int comp, bool done) { this->comp = comp, this->done = done; }
	done_info() { this->comp = 0, this->done = 0; }
};

struct concurrent_trie_node
{
	A(done_info) dinf = { done_info() };
	A(concurrent_trie_node*)* ch;
	concurrent_trie_node* p;
	static bool strict;
	int mxd, mxb;
	vertex val;
	int d;
	concurrent_trie_node(concurrent_trie_node* p, vertex val);
	concurrent_trie_node(int mxd, int mxb);
	~concurrent_trie_node();
};

void set_dinf(concurrent_trie_node*, done_info);

void insert_path(list<vertex>::iterator, list<vertex>::iterator, concurrent_trie_node*, bool);
concurrent_trie_node* find(list<vertex>::iterator, list<vertex>::iterator, concurrent_trie_node*);

#endif