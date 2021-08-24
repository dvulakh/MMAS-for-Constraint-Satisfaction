
#include "concurrent_trie_node.h"

typedef list<vertex>::iterator lvi;
typedef concurrent_trie_node trie;

// Construct node
trie::concurrent_trie_node(trie* p, vertex val)
{
    this->d = p->d + 1;
    this->mxd = p->mxd;
    this->mxb = p->mxb;
    this->ch = new A(trie*)[mxb + 1]();
    this->dinf = done_info(d == mxd, d == mxd);
    this->val = val;
    this->p = p;
}
trie::concurrent_trie_node(int mxd, int mxb)
{
    this->ch = new A(trie*)[mxb + 1]();
    this->dinf = done_info(0, 0);
    this->mxb = mxb;
    this->mxd = mxd;
    this->p = NULL;
    this->val = 0;
    this->d = 0;
}

// Delete node
trie::~concurrent_trie_node()
{
    for (int i = 0; i < mxb; i++)
        if (ch[i] != NULL)
            delete ch[i];
    delete[] ch;
}

// Set done_info
void set_dinf(trie* t, done_info d)
{
    done_info nd = d, od = d;
    do {
        nd = d;
        od = t->dinf.load();
    } while (!t->dinf.compare_exchange_weak(od, nd));
}

// Insert path
void insert_path(lvi walk, lvi end, trie* t, bool kill)
{
    if (t->d == t->mxd)
        return;
    vertex v = *walk;
    if (t->ch[v].load() == NULL) {
        trie* nt = new trie(t, v), * ot = NULL;
        t->ch[v].compare_exchange_weak(ot, nt);
    }
    walk++;
    if (walk == end) {
        if (kill)
            set_dinf(t, done_info(1, 1));
        return;
    }
    insert_path(walk, end, t->ch[v], kill);
    int dch = 0; double comp = 0;
    for (int i = 0; i <= t->mxb; i++)
        if (t->ch[i].load() != NULL)
            comp += t->ch[i].load()->dinf.load().comp, dch += t->ch[i].load()->dinf.load().done;
    done_info od = t->dinf, nd = t->dinf;
    do {
        od = t->dinf;
        nd = done_info(max(0 + od.comp, comp / (t->mxb - t->d)), od.done || dch == t->mxb - t->d);
    } while (!t->dinf.compare_exchange_weak(od, nd));
}
 
// Follow path
trie* find(lvi walk, lvi end, trie* t)
{
    if (walk == end)
        return t;
    vertex v = *walk++;
    return t->ch[v].load() == NULL ? NULL : find(walk, end, t->ch[v].load());
}