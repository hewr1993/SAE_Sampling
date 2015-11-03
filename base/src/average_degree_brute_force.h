#include "storage/mgraph.h"
#include "solver/solver.h"

class average_degree_brute_force:public sae::Solver<int>{
public:
      average_degree_brute_force(sae::io::MappedGraph *graph);
      ~average_degree_brute_force();
      double solve(std::string str);
};
