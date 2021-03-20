mkdir ../bin

g++ ../src/scip_wrapper.cpp ../examples/vertex_cover.cpp -L/usr/local/lib -lscip -o ../bin/minimum_vertex_cover_test
g++ ../src/scip_wrapper.cpp ../examples/independent_set.cpp -L/usr/local/lib -lscip -o ../bin/independent_set_test
g++ ../src/scip_wrapper.cpp ../examples/dependency_knapsack.cpp -L/usr/local/lib -lscip -o ../bin/dependency_knapsack_test
g++ ../src/scip_wrapper.cpp ../examples/n_queens_completion.cpp -L/usr/local/lib -lscip -o ../bin/n_queens_completion_test
g++ ../src/scip_wrapper.cpp ../examples/min_cost_flow.cpp -L/usr/local/lib -lscip -o ../bin/min_cost_flow_test
g++ ../src/scip_wrapper.cpp ../examples/min_team_matching.cpp -L/usr/local/lib -lscip -o ../bin/min_team_matching_test
g++ ../src/scip_wrapper.cpp ../examples/longest_path_catan.cpp -L/usr/local/lib -lscip -o ../bin/longest_path_catan_test
g++ ../src/scip_wrapper.cpp ../examples/tsp.cpp -L/usr/local/lib -lscip -o ../bin/tsp_test
g++ ../src/scip_wrapper.cpp ../examples/chromatic_number.cpp -L/usr/local/lib -lscip -o ../bin/chromatic_number_test
g++ ../src/scip_wrapper.cpp ../examples/three_partition.cpp -L/usr/local/lib -lscip -o ../bin/three_partition_test
g++ ../src/scip_wrapper.cpp ../examples/max_clique.cpp -L/usr/local/lib -lscip -o ../bin/max_clique_test
g++ ../src/scip_wrapper.cpp ../examples/weighted_max_cut.cpp -L/usr/local/lib -lscip -o ../bin/weighted_max_cut_test


g++ ../src/scip_wrapper.cpp ../examples/trivial_examples.cpp -L/usr/local/lib -lscip -o ../bin/trivial_examples_test
# run using: (to find the shared library)
# LD_LIBRARY_PATH=/usr/local/lib ./...
