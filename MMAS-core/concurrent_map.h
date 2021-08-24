
#ifndef CON_MAP
#define CON_MAP

#include "multithread.h"
#include <set>

// Single-write safe concurrent map: find, insert, delete, and iterate, as long as at most one thread writes or iterates
template<typename K, typename V>
class concurrent_map
{
public: struct iterator;
private:
    /// Iterator types
    const static int BEGIN = 1;
    const static int END = 2;
    /// Hidden iterator structures
    struct wrapped_iterator;
    struct access_guard_na;
    struct internal_map;
    friend wrapped_iterator;
    /// Double-reference access guard
    struct access_guard_na
    {
        /// Next iterator
        wrapped_iterator* wit;
        /// Number of threads that have acquired next
        int count;
        /// Whether new threads can acquire next
        bool inc;
        /// Constructor: default count = 1
        access_guard_na(wrapped_iterator*);
        access_guard_na();
    };
    typedef atomic<access_guard_na> access_guard;
    /// List node
    struct wrapped_iterator
    {
        /// Parent map
        internal_map* mmap;
        /// Key-Value pair
        pair<K, V> keyval;
        /// Access guard to next pointer
        access_guard nxt = { access_guard_na() };
        /// Wrapping iterator
        iterator mit;
        /// Marks if begin or end iterator
        int itype;
        /// Stores bucket sz
        int bucket_sz;
        /// End iterator (stored by beginnings of lists)
        wrapped_iterator* end;
        /// Internal released thread counter
        INT intr;
        /// Constructor given key and value
        wrapped_iterator(K, V, internal_map*);
        wrapped_iterator(internal_map*);
        /// Acquire next iterator; user must release after finishing
        wrapped_iterator* acquire_next();
        /// Release iterator access
        void release();
    };
    /// Make and destroy list
    static void unsafe_destroy_list(wrapped_iterator*);
    wrapped_iterator* make_list();
    /// Hash function
    long long(*hash_func)(const K&);
    /// Hash table
    struct internal_map
    {
        /// Table information
        wrapped_iterator* intbegin, * intend;
        set<pair<int, int>, bool(*)(const pair<int, int>&, const pair<int, int>&)> buckets;
        wrapped_iterator** table;
        int tblsz;
        int sz;
        /// Atomic increment functions
        wrapped_iterator* atomic_compare_and_increment(wrapped_iterator*);
        wrapped_iterator* atomic_compare_and_add(wrapped_iterator*, int);
        void atomic_set_inc(wrapped_iterator*, bool);
        /// Destructor
        ~internal_map();
    };
    internal_map* imap;
    /// Search hash table
    pair<wrapped_iterator*, wrapped_iterator*> internal_find(const K&);
public:
    /// External iterator wraps internal node pointer
    class iterator
    {
        friend concurrent_map;
    private:
        wrapped_iterator* it;
    public:
        bool operator==(const iterator&);
        bool operator!=(const iterator&);
        iterator(wrapped_iterator*);
        iterator operator++(int);
        pair<K, V>* operator->();
        pair<K, V>& operator*();
        iterator unpad();
        void release();
        iterator();
    };
    /// Constructors
    concurrent_map(long long(*)(const K&), int);
    concurrent_map(long long(*)(const K&));
    void init(long long(*)(const K&), int);
    concurrent_map() {}
    /// Destructor
    void destroy_internals();
    /// Retrieve beginning and end iterators
    iterator begin();
    iterator end();
    /// Retrieve largest bucket size
    pair<int, int> max_bucket();
    /// Insert key pair
    iterator insert_and_return(pair<K, V>);
    void insert(pair<K, V>);
    /// Find key pair
    iterator find(const K&);
    /// Erase key pair
    iterator erase_and_return(K&);
    void erase(K&);
    /// Query size
    int size();
};

bool concurrent_map_bucket_compare(const pair<int, int>& b1, const pair<int, int>& b2);

#endif

//#include "concurrent_map.h"
#define DEFAULT_SZ 1299827

/*** CONDITIONAL COUNTER ACCESS GUARD ***/

// Constructors
template<typename K, typename V>
concurrent_map<K, V>::access_guard_na::access_guard_na(concurrent_map<K, V>::wrapped_iterator* wit)
{
    this->inc = true;
    this->wit = wit;
    this->count = 0;
}
template<typename K, typename V>
concurrent_map<K, V>::access_guard_na::access_guard_na()
{
    this->inc = true;
    this->wit = NULL;
    this->count = 0;
}

// Compare and add
template<typename K, typename V>
typename concurrent_map<K, V>::wrapped_iterator* concurrent_map<K, V>::internal_map::atomic_compare_and_increment(concurrent_map<K, V>::wrapped_iterator* count) { return atomic_compare_and_add(count, 1); }
template<typename K, typename V>
typename concurrent_map<K, V>::wrapped_iterator* concurrent_map<K, V>::internal_map::atomic_compare_and_add(concurrent_map<K, V>::wrapped_iterator* count, int delta)
{
    concurrent_map<K, V>::access_guard_na oldcount = count->nxt.load(), newcount;
    do {
        oldcount.inc = true;
        newcount = oldcount;
        newcount.count += delta;
    } while (!count->nxt.compare_exchange_weak(oldcount, newcount));
    return oldcount.wit;
}

// Compare and set inc
template<typename K, typename V>
void concurrent_map<K, V>::internal_map::atomic_set_inc(concurrent_map<K, V>::wrapped_iterator* count, bool inc)
{
    concurrent_map<K, V>::access_guard_na oldcount = count->nxt.load(), newcount;
    do {
        newcount = oldcount;
        newcount.inc = inc;
    } while (!count->nxt.compare_exchange_weak(oldcount, newcount));
}

/*** CONCURRENT MAP: WRAPPED ITERATOR ***/

// Wrapped iterator construction
template<typename K, typename V>
concurrent_map<K, V>::wrapped_iterator::wrapped_iterator(K key, V val, concurrent_map<K, V>::internal_map* mmap)
{
    this->keyval = { key, val };
    this->mit = iterator(this);
    this->mmap = mmap;
    this->end = NULL;
    this->intr = 0;
}
template<typename K, typename V>
concurrent_map<K, V>::wrapped_iterator::wrapped_iterator(concurrent_map<K, V>::internal_map* mmap)
{
    this->keyval = { K(), V() };
    this->mit = iterator(this);
    this->mmap = mmap;
    this->end = NULL;
    this->intr = 0;
}

// Acquire next iterator
template<typename K, typename V>
typename concurrent_map<K, V>::wrapped_iterator* concurrent_map<K, V>::wrapped_iterator::acquire_next() { return this == mmap->intend ? mmap->intend : mmap->atomic_compare_and_increment(this); }

// Release control of iterator
template<typename K, typename V>
void concurrent_map<K, V>::wrapped_iterator::release() { this->intr++; }

// Make empty list
template<typename K, typename V>
typename concurrent_map<K, V>::wrapped_iterator* concurrent_map<K, V>::make_list()
{
    wrapped_iterator* it = new wrapped_iterator(imap);
    access_guard_na nxt = it->nxt.load();
    nxt.wit = new wrapped_iterator(imap);
    it->itype = BEGIN, nxt.wit->itype = END;
    it->bucket_sz = 0;
    it->end = nxt.wit;
    it->nxt = nxt;
    return it;
}

// Destroy list -- unsafe if other threads accessing
template<typename K, typename V>
void concurrent_map<K, V>::unsafe_destroy_list(concurrent_map<K, V>::wrapped_iterator* wit)
{
    concurrent_map<K, V>::wrapped_iterator* nxt;
    while (wit->itype != END) {
        nxt = wit->nxt.load().wit;
        delete wit;
        wit = nxt;
    }
    delete wit;
}

/*** CONCURRENT MAP: EXTERNAL ITERATOR ***/

// Constructor
template<typename K, typename V>
concurrent_map<K, V>::iterator::iterator(concurrent_map<K, V>::wrapped_iterator* it) { this->it = it; }
template<typename K, typename V>
concurrent_map<K, V>::iterator::iterator() { this->it = NULL; }

// Release
template<typename K, typename V>
void concurrent_map<K, V>::iterator::release() { it->release(); }

// Leave padding
template<typename K, typename V>
typename concurrent_map<K, V>::iterator concurrent_map<K, V>::iterator::unpad()
{
    while ((it->itype == BEGIN || it->itype == END) && it != it->mmap->intend) {
        concurrent_map<K, V>::wrapped_iterator* nxt = it->acquire_next();
        it->release();
        it = nxt;
    }
    return it->mit;
}

// Overloaded operators
/// Equality
template<typename K, typename V>
bool concurrent_map<K, V>::iterator::operator==(const concurrent_map<K, V>::iterator& it) { return it.it == this->it; }
/// Inequality
template<typename K, typename V>
bool concurrent_map<K, V>::iterator::operator!=(const concurrent_map<K, V>::iterator& it) { return it.it != this->it; }
/// Increment
template<typename K, typename V>
typename concurrent_map<K, V>::iterator concurrent_map<K, V>::iterator::operator++(int x)
{
    do {
        concurrent_map<K, V>::wrapped_iterator* nxt = it->acquire_next();
        it->release();
        it = nxt;
    } while ((it->itype == BEGIN || it->itype == END) && it != it->mmap->intend);
    return it->mit;
}
/// Dereference
template<typename K, typename V>
pair<K, V>& concurrent_map<K, V>::iterator::operator*() { return it->keyval; }
/// Arrow
template<typename K, typename V>
pair<K, V>* concurrent_map<K, V>::iterator::operator->() { return &it->keyval; }

// Begin and end iterators
template<typename K, typename V>
typename concurrent_map<K, V>::iterator concurrent_map<K, V>::begin() {
    iterator it = imap->intbegin->mit;
    it++;
    return it.it->mit;
}
template<typename K, typename V>
typename concurrent_map<K, V>::iterator concurrent_map<K, V>::end() { return imap->intend->mit; }

/*** CONCURRENT MAP: INSERTING, DELETING, SEARCHING ***/

// Insert key-value pair to map
template<typename K, typename V>
typename concurrent_map<K, V>::iterator concurrent_map<K, V>::insert_and_return(pair<K, V> keyval)
{
    /// Construct new iterator
    wrapped_iterator* it = new wrapped_iterator(keyval.first, keyval.second, imap);
    /// Compute hash value
    int hash = hash_func(keyval.first) % imap->tblsz;
    /// Acquire last non-end iterator
    wrapped_iterator* cit = imap->table[hash], * nit = cit->acquire_next();
    bool empty = nit->itype == END;
    while (nit->itype != END) {
        cit->release(), cit = nit, nit = nit->acquire_next();
        if (cit->keyval.first == keyval.first) {
            cit->release(), nit->release();
            return end();
        }
    }
    /// Set end iterator as next of it
    it->nxt = cit->nxt.load();
    /// Set it as next of previously last iterator
    cit->nxt = access_guard_na(it);
    /// Release iterator
    cit->release();
    /// Increment size
    imap->buckets.erase({ hash, imap->table[hash]->bucket_sz });
    imap->table[hash]->bucket_sz++;
    imap->buckets.insert({ hash, imap->table[hash]->bucket_sz });
    imap->sz++;
    /// Add to map's overall iterator list if necessary
    if (empty) {
        imap->table[hash]->end->nxt = imap->intbegin->nxt.load();
        imap->intbegin->nxt = access_guard_na(imap->table[hash]);
        imap->table[hash]->end->acquire_next()->end = imap->table[hash]->end;
        imap->table[hash]->end = imap->intbegin;
    }
    return it->mit;
}
template<typename K, typename V>
void concurrent_map<K, V>::insert(pair<K, V> keyval)
{
    iterator it = insert_and_return(keyval);
    it.it->release();
}

// Internal search returns pair: preceeding iterator and desired iterator
template<typename K, typename V>
pair<typename concurrent_map<K, V>::wrapped_iterator*, typename concurrent_map<K, V>::wrapped_iterator*> concurrent_map<K, V>::internal_find(const K& key)
{
    /// Compute hash value
    int hash = hash_func(key) % imap->tblsz;
    /// Iterate through list container
    wrapped_iterator* cit = imap->table[hash], * nit = cit->acquire_next();
    while (nit->itype != END && nit->keyval.first != key)
        cit->release(), cit = nit, nit = nit->acquire_next();
    /// Return pair
    return { cit, nit };
}

// Public search returns only found iterator
template<typename K, typename V>
typename concurrent_map<K, V>::iterator concurrent_map<K, V>::find(const K& key)
{
    pair<wrapped_iterator*, wrapped_iterator*> p = internal_find(key);
    p.first->release();
    return p.second->itype == END ? end() : p.second->mit;
}

// Delete key-value pair from map
template<typename K, typename V>
typename concurrent_map<K, V>::iterator concurrent_map<K, V>::erase_and_return(K& key)
{
    /// Find iterator with desired key
    pair<wrapped_iterator*, wrapped_iterator*> p = internal_find(key);
    if (p.second->itype == END) {
        p.first->release();
        return end();
    }
    /// Block access to iterator
    imap->atomic_set_inc(p.first, false);
    p.second->intr -= p.first->nxt.load().count;
    /// Wait for all threads to release
    int newval, oldval;
    do { newval = 1, oldval = 0; } while (!p.second->intr.compare_exchange_weak(oldval, newval));
    /// Replace previous link
    imap->atomic_set_inc(p.second, false);
    p.first->nxt = p.second->nxt.load();
    imap->atomic_set_inc(p.first, true);
    /// Release previous node and delete iterator
    wrapped_iterator* ans = p.first->acquire_next();
    int hash = hash_func(key) % imap->tblsz;
    p.first->release();
    delete p.second;
    /// Decrement size
    imap->buckets.erase({ hash, imap->table[hash]->bucket_sz });
    imap->table[hash]->bucket_sz--;
    imap->buckets.insert({ hash, imap->table[hash]->bucket_sz });
    imap->sz--;
    /// Remove from overall list
    wrapped_iterator* nwit = imap->table[hash]->acquire_next();
    if (nwit->itype == END) {
        imap->table[hash]->end->nxt = nwit->nxt.load();
        nwit->acquire_next()->end = imap->table[hash]->end;
        imap->table[hash]->end = nwit;
    }
    else
        nwit->release();
    /// Return
    return ans->mit;
}
template<typename K, typename V>
void concurrent_map<K, V>::erase(K& key)
{
    iterator it = erase_and_return(key);
    it.it->release();
}

// Query size
template<typename K, typename V>
int concurrent_map<K, V>::size() { return imap->sz; }
template<typename K, typename V>
pair<int, int> concurrent_map<K, V>::max_bucket() { return imap->buckets.size() ? *imap->buckets.begin() : pair<int, int>({ 0, 0 }); }

/*** CONCURRENT MAP: CONSTRUCTORS AND DESTRUCTORS ***/

// Initialize
template<typename K, typename V>
void concurrent_map<K, V>::init(long long(*hash_func)(const K&), int tblsz)
{
    this->imap = new internal_map();
    this->imap->table = new wrapped_iterator * [tblsz];
    this->imap->buckets = set<pair<int, int>, bool(*)(const pair<int, int>&, const pair<int, int>&)>(concurrent_map_bucket_compare);
    this->hash_func = hash_func;
    this->imap->tblsz = tblsz;
    for (int i = 0; i < tblsz; i++)
        imap->table[i] = make_list();
    this->imap->intbegin = make_list();
    this->imap->intend = imap->intbegin->nxt.load().wit;
}

// Hash function constructor
template<typename K, typename V>
concurrent_map<K, V>::concurrent_map(long long(*hash_func)(const K&)) { init(hash_func, DEFAULT_SZ); }

// Size and hash constructor
template<typename K, typename V>
concurrent_map<K, V>::concurrent_map(long long(*hash_func)(const K&), int tblsz) { init(hash_func, tblsz); }

// Destroy
template<typename K, typename V>
concurrent_map<K, V>::internal_map::~internal_map()
{
    buckets.clear();
    for (int i = 0; i < tblsz; i++)
        unsafe_destroy_list(table[i]);
    delete intbegin;
    delete[] table;
    delete intend;
}
template<typename K, typename V>
void concurrent_map<K, V>::destroy_internals() { delete imap; }
