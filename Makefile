CPPFLAGS=-g -I src/ -I examples/ -lscip
LDFLAGS=-g -L/usr/local/lib -lscip
LDLIBS=-L/usr/local/lib -lscip

ODIR=obj
BINDIR=bin

SRCS=$(ODIR)/scip_wrapper.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

milp   = chromatic_number  dependency_knapsack independent_set longest_path_catan max_clique min_cost_flow min_team_matching	\
						n_queens_completion rectangle_packing three_partition trivial_examples tsp vertex_cover weighted_max_cut
milp_obj_files = chromatic_number.o dependency_knapsack.o independent_set.o longest_path_catan.o max_clique.o min_cost_flow.o min_team_matching.o	\
						n_queens_completion.o rectangle_packing.o three_partition.o trivial_examples.o tsp.o vertex_cover.o weighted_max_cut.o

qubo = qubo_vertex_cover

qubo_obj_files = qubo_vertex_cover.o


all: $(milp) $(qubo)

clean:
	rm bin/*
	rm obj/*

$(milp): %: %.o scip_wrapper.o
	g++ $(LDFLAGS) -o $(BINDIR)/$@ $(ODIR)/$@.o $(OBJS) $(LDLIBS)


$(filter %.o,$(milp_obj_files)): %.o: examples/milp_examples/%.cpp scip_wrapper.o
	g++ $(CPPFLAGS) -c examples/milp_examples/$(basename $@).cpp -o $(ODIR)/$@


$(qubo): %: %.o scip_wrapper.o
	g++ $(LDFLAGS) -o $(BINDIR)/$@ $(ODIR)/$@.o $(OBJS) $(LDLIBS)


$(filter %.o,$(qubo_obj_files)): %.o: examples/qubo_examples/%.cpp scip_wrapper.o
	g++ $(CPPFLAGS) -c examples/qubo_examples/$(basename $@).cpp -o $(ODIR)/$@


scip_wrapper.o: src/scip_wrapper.cpp src/scip_wrapper.hpp
	g++ $(CPPFLAGS) -c src/scip_wrapper.cpp -o $(ODIR)/scip_wrapper.o