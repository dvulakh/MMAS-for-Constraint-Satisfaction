
# Use bash
SHELL := /bin/bash

# Template file name
TEMPLATE = .TEMPLATE

# Main program name
CONF = problem_config.h
PSET = ./.problem-settings
CORE = ./MMAS-core
NAME = MMAS

# Target executable name
PROBS?=./problem-graphs
EXEC = mmas
PROG?= template
SUFF?= _graph
CLASS?=$(PROG)$(SUFF)_map

# Compile recipe
compile : bin/$(EXEC)-$(PROG)
bin/$(EXEC)-$(PROG) : $(PSET)/$(PROG).h
	@mkdir -p bin
	@g++ -w -latomic -pthread -std=c++17 $(CORE)/*.cpp \
	$(PROBS)/$(PROG)$(SUFF).cpp -o bin/$(EXEC)-$(PROG)
# Install recipe
install : /usr/share/bin/$(EXEC)-$(PROG)
/usr/share/bin/$(EXEC)-$(PROG) : $(PSET)/$(PROG).h
	@g++ -w -latomic -pthread -std=c++17 $(CORE)/*.cpp \
	$(PROBS)/$(PROG)$(SUFF).cpp -o /usr/share/bin/$(EXEC)-$(PROG)
uninstall :
	@rm -r /usr/share/bin$(EXEC)*
# Problem header recipe (prereq)
$(PSET)/$(PROG).h : $(CORE)/* $(PROBS)/$(PROG)$(SUFF).h $(PROBS)/$(PROG)$(SUFF).cpp
	@mkdir -p $(PSET)
	@printf "/*** PROBLEM_CONFIG.H: MAKEFILE-GENERATED ***/\n\n\
	#ifndef P_CONFIG_H\n#define P_CONFIG_H\n\n\
	#include \"../$(PROBS)/$(PROG)$(SUFF).h\"\n\
	typedef $(CLASS) problem_to_solve;\n\n#endif" >	$(PSET)/$(PROG).h
	@cp $(PSET)/$(PROG).h $(PSET)/$(CONF)
# Template creation recipe
template :
	@mkdir -p $(PROBS)
	@[ ! -f $(PROBS)/$(PROG)$(SUFF).cpp ] && cat $(CORE)/$(TEMPLATE).cpp |\
	sed "s/PROG/$(PROG)/g; s/SUFF/$(SUFF)/g; s/CLASS/$(CLASS)/g" >\
	$(PROBS)/$(PROG)$(SUFF).cpp ||\
	echo "Error: $(PROBS)/$(PROG)$(SUFF).cpp already exists!"
	@[ ! -f $(PROBS)/$(PROG)$(SUFF).h ] && cat $(CORE)/$(TEMPLATE).h |\
	sed "s/UPROG/`echo $(PROG) | tr [a-z] [A-Z]`/g;\
	s/PROG/$(PROG)/g; s/SUFF/$(SUFF)/g; s/CLASS/$(CLASS)/g" >\
	$(PROBS)/$(PROG)$(SUFF).h ||\
	echo "Error: $(PROBS)/$(PROG)$(SUFF).h already exists!"