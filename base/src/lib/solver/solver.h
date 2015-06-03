#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "storage/mgraph.h"

namespace sae {
/*
 * specific algorithm applied to graph data
 */
template <class return_t>
class Solver {
    public:
		Solver(sae::io::MappedGraph *graph) {
			this->graph = graph;
		}
		~Solver() {
			//delete graph;
		}
		virtual return_t solve() {} // main process
		virtual return_t solve(sae::io::vid_t) {} // online method (vertex)
		virtual return_t solve(sae::io::vid_t, sae::io::vid_t) {} // online method (edge)
	private:
		Solver(const Solver&) {}
	protected:
		sae::io::MappedGraph *graph;
};
}

#endif
