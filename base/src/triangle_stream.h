#include "storage/mgraph.h"

class Triangle_Stream {
public:
	Triangle_Stream(int se, int sw); // size of edges / wedges
	~Triangle_Stream();
	int count(sae::io::vid_t, sae::io::vid_t); // return exact number of triangles
private:
	void update(sae::io::vid_t, sae::io::vid_t);

	int edge_count; // how many edges processed
	int se, sw;
	sae::io::vid_t **edge_res; // [][2] indexes of an edge
	sae::io::vid_t **wedge_res; // [][3] indexes of a wedge 0->1->2
	bool *isClosed; // related to wedge_res
	int tot_wedges; // number of wedges formed by edge_res
};
