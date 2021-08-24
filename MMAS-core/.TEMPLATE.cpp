// Include your header
#include "PROGSUFF.h"

// Return the cost of adding new vertex to path (to be used as heta in MMAS).
// Argument #1 (walk) is the current (incomplete) path constructed by the ants.
// Argument #2 (nxt) is the vertex being considered for next element.
// 
// Arguments are passed by reference to save time; neither should be modified.
// Argument #1 is type path, which by default is pair<list<vertex>*, cost>
// Argument #2 is type vertex, which by default is int
// Type cost is also int
// The first element of the path pair is a pointer to a list of vertices
// visited so far, in the order they have been visited (that is, the prefix of
// a solution candidate permutation).
// The second element of the path pair is the cost incurred so far.
// 
// If these type aliases are not suitable (for example, if you need fractional
// costs), you can change them in the source file construction_graph.h --
// however, beware that you may then have to modify the source files of the
// ant system and map-based construction graph.
cost PROGSUFF::cost_of(path& walk, vertex& nxt)
{
    // Return the cost
    return 0;
}

// Return whether new vertex may be added to path (hard constraint in MMAS).
// Argument #1 (walk) is the current (incomplete) path constructed by the ants.
// Argument #2 (nxt) is the vertex being considered for next element.
// 
// Arguments are passed by reference to save time; neither should be modified.
// See above for descriptions of argument types and contents.
// 
// The hard constraints must be loose; that is, for any argument walk that does
// not violate the hard constraints, there should exist a vertex nxt that can
// be added to the walk.
bool PROGSUFF::hard_constraint(path& walk, vertex& nxt)
{
    // Right now, no hard constraints
    return true;
}

// The constructor of your problem graph.
// Argument #1 (nvar) is the number of variables (permutation length) requested.
// Argument #2 (max_val) is the domain size requested (path will have elements
//                       in the range (1..max_val) )
// Argument #3 (plen) is the pheromone tail length requested.
// Argument #4 (cons) is the name of a file containing constraint information.
CLASS::CLASS(int nvar, int max_val, int plen, string cons) :
    // Constructor chain: let the map graph ancestor set up what it needs to.
    // Forward the number of variables and the domain size to the map graph
    // constructor. Note that you do not *have* to use the user-requested
    // values for both (see costas_graph.cpp and sat_graph.cpp for examples).
    construction_graph_map(nvar, max_val, plen) {
    // If relative pheromone storage can be applied to your problem, set this
    // to true. Relative pheromone storage will associate pheromone infor-
    // mation with a permutation's difference array rather than the permut-
    // ation itself.
    this->relative = false;
    // If circular pheromone storage can be applied to your problem, set this
    // to true. Circular pheromone storage will associate pheromone infor-
    // mation only with sequences of values and not their location in the
    // permutation.
    this->circular = false;
    // If you need to read constraints from a file, use the path cons.
    // See sat_graph.cpp and gcol_graph.cpp for examples.
    // (read info from cons)
}