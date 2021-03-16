g++ ../src/scip_wrapper.cpp ../src/vertex_cover.cpp -L/usr/local/lib -lscip -o minimum_vertex_cover_test
g++ ../src/scip_wrapper.cpp ../src/independent_set.cpp -L/usr/local/lib -lscip -o independent_set_test
g++ ../src/scip_wrapper.cpp ../src/dependency_knapsack.cpp -L/usr/local/lib -lscip -o dependency_knapsack_test
g++ ../src/scip_wrapper.cpp ../src/n_queens.cpp -L/usr/local/lib -lscip -o n_queens_test
g++ ../src/scip_wrapper.cpp ../src/min_cost_flow.cpp -L/usr/local/lib -lscip -o min_cost_flow_test
# run using: (to find the shared library)
# LD_LIBRARY_PATH=/usr/local/lib ./minimum_vertex_cover_test
# LD_LIBRARY_PATH=/usr/local/lib ./independent_set_test
# LD_LIBRARY_PATH=/usr/local/lib ./dependency_knapsack_test
