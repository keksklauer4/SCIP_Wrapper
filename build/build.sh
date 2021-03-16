g++ ../src/scip_wrapper.cpp ../src/vertex_cover.cpp ../src/test.cpp -L/usr/local/lib -lscip -o minimum_vertex_cover_test

# run using: (to find the shared library)
# LD_LIBRARY_PATH=/usr/local/lib ./minimum_vertex_cover_test
