# mDRACO for Constraint Satisfaction
Parallel m-Dimensional Relative Ant Colony Optimization (mDRACO) for
solving constraint-satisfaction problems like SAT, graph coloring, and
Costas arrays  

## General Usage
We provide the C++ source code for a general mDRACO framework that
uses map-based pheromone storage. Users can specify a problem by
writing a short program that determines the cost (number of constraint
violations) of an integer sequence representing a path through the
construction graph. The framework traverses the graph described by the
cost function in search of optimal paths (solutions to the problem).  

## Makefile and Compilation

### Makefile Variables
PROG:           Name of the program that specifies the graph mDRACO
                traverses  
                Usually the name of the problem the graph describes
                (e.g. SAT)  
                (default `"template"`)  
SUFF:           Suffix of the program source files  
                (default `"_graph"`)  
CLASS:          Name of the graph class that implements the
                user-defined cost and a pheromone-storage mechanism
                from the framework  
                (default `"$(PROG)$(SUFF)_map"`)  
PROBS:          Path to the directory that contains source files that
                specify construction graphs for mDRACO  
                (default `"./problem-graphs"`)  

### Makefile Recipes
compile:        The default recipe  
                Compiles mDRACO to solve the problem specified by
                `$(PROG)`  
                Links the files `$(PROBS)/$(PROG)$(SUFF)(.h|.cpp)`,
                then compiles the source in ./MMAS-core and writes an
                executable named `mmas-$(PROG)` to ./bin/  
install:        Same as compile, but writes the executable to
                /usr/share/bin/  
template:       Creates template source files named
                `$(PROG)$(SUFF)(.h|.cpp)` in
                `$(PROBS)`. These files contain the method headers
                necessary to specify a graph class that mDRACO can use
                to solve constraint problems.  
uninstall:      Removes all files beginning with mmas from
                `/usr/share/bin/`  

### Examples
Compile the example Costas-array program provided in
`./problem-graphs`  
```make PROG=costas```  
Create templates with which the user can specify a new problem
(nqueens) and place them in a non-default directory
(`~/mmas-templates`)  
```make template PROG=nqueens PROBS=~/mmas-templates```  
Install the completed nqueens solver to `/usr/share/bin/`  
```make install PROG=nqueens PROBS=~/mmas-templates```  

## Execution options

### Ant System Options
The following options can be used to change the settings of the ant
system when an mDRACO executable is invoked from the command line.  
  
--threads  -n   [positive integer]  
                Number of worker threads (default 4)  
--lambda   -L   [integer]  
                Trail length value λ, the suffix length for pheromone
                association; λ < 1 enables arbitrary-length pheromone
                association (default -1)  
--ants     -N   [positive integer]  
                Number of ants in the colony; queen performs pheromone
                updates after every N iterations (default 10)  
--alpha    -A   [real number]  
                Relative weight α of pheromone value τ on ant behavior
                (default 2)  
--beta     -B   [real number]  
                Relative weight β of heuristic value η on ant behavior
                (default 25)  
--gamma    -G   [real number]  
                Relative weight γ of trail length value λ on ant
                behavior
                (default 0.1)  
--rho      -v   [real number]  
                Rate of pheromone evaporation ρ (default 0.01)  
--p0       -p   [real number]  
                Probability p₀ of automatically choosing
                highest-pheromone path (default 0)  
--tmin          [real number]  
                Lowest possible pheromone value (default 2)  
--tmax          [real number]  
                Highest possible pheromone value (default 10)  
--theta    -Q   [integer]  
                Quality threshold ϑ; ϑ < 1 disables quality threshold
                (default 5)  
--regicide -t   [integer]  
                Time limit; mDRACO will kill queen process after t
                seconds, even if no solution is found; no time limit
                if t < 0
                (default -1)  
--restart  -R  
                Restart from file  
--map      -M  
                Log pheromone map entries  
--fout     -r   [file path]  
                Path to restart file; mDRACO will restart from here if
                the -R flag is used and will append new map entries if
                the -M flag is used
                (default "mmas.out")  
--period   -T   [positive integer]  
                Queen logs map once every T pheromone updates
                (default 1)  
--data     -D  
                Log individual ant path data  
--work     -W  
                Log detailed information about individual ant work  
--trie  
                Insert ant paths into trie (not recommended)  

### Graph Options
The following options can be used to pass information about a problem
size or instance to the user-defined graph program when an mDRACO
executable is invoked from the command line.  
  
--size     -l   [positive integer]  
                Problem size; solution candidates will be integer
                sequences of length l  
--domain   -m   [positive integer]  
                Problem domain; solution candidates will be sequences
                of the integers (1..m)  
--cons     -C   [file path]
                Path to constraint file (for problems that read
                additional constraint info from file; default
                "cons.in")  

### Examples
Run the costas-solver from ./bin to search for size 32 arrays on 200
processors and log the output to "~/mmas-logs/costas-32.out" every 100
periods  
```bin/mmas-costas -nm 200 32 -MrT ~/mmas-logs/costas-32.out 100```  
Run an installed MMAS executable with custom pheromone weight and
range using fixed trail length 4  
```mmas-gcol --tmin 0.5 -ABL 5 10 4 --tmax 1.5```  

## Creating Custom Graph Programs

### General Instructions
Users can apply the mDRACO framework to new constraint problems by
specifying new graphs for the ants to traverse. The existence of edges
between states corresponds to the hard constraints of the problem, and
the weight of those edges corresponds to the cost incurred by
traversing them (for constraint- satisfaction problems, usually the
number of new soft-constraint violations). To start preparing a new
graph program, run `make template` (optionally setting the PROG
variable to the problem name; see the section titled "Makefile and
Compilation") and implement the bodies of the methods as described in
the comments of the templates. See the files in ./problem-graphs for
examples.  

### Type definitions
The mDRACO framework aliases some of the types used in describing
pheromone information and graph properties. The standard aliases are
listed below.  

```
typedef int cost;
typedef int vertex;
typedef std::pair<std::list<vertex>*, cost> path;
typedef double pheromone;
```
  
The user can change these aliases. However, it is not guaranteed that
the mDRACO framework will continue to work correctly if changes are
made, and it may be necessary for the user to edit the source files in
./MMAS-core  
