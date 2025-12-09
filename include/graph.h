#ifndef GRAPH_H
#define GRAPH_H

#include "../include/dynamic_structs.h"


typedef struct Vertex {
   char *name;
   Rule *rule;
   struct Vertex **deps;
   int dep_count;
   int dep_cap;
   int color;             // 0 = white, 1 = gray, 2 = black
} Vertex;

typedef struct {
   Vertex **vertices;
   int count;
   int cap;
} Graph;


typedef enum { 
   GRAPH_OK = 0, 
   GRAPH_ERR_NOMEM = -1, 
   GRAPH_ERR_CYCLE = -2,
	GRAPH_ALREADY_IN_STACK = -3
} graph_status;

typedef enum {
   WHITE,
   GRAY,
   BLACK
} vertex_color;

void graph_init(Graph *g);
void graph_free(Graph *g);

/* Adds edge (from -> to). If there are no vertices, creates them. 
Returns GRAPH_OK or GRAPH_ERR_NOMEM. */
int graph_add_edge(Graph *g, const char *from_name, const char *to_name);

// Find vertex by name or create it if not found. Return
Vertex *graph_find_or_create(Graph *g, const char *name, int *err);

/* Build a topological sort.
   If successful, returns the GRAPH_OK and fills in the out_order with the number of out_n.
   out_order is allocated via malloc - the caller must free(out_order)
   If there is a loop, returns GRAPH_ERR_CYCLE. */
int graph_topo_sort(Graph *g, Vertex ***out_order, int *out_n);

/* Simple check: is there a loop in a graph*/
int graph_has_cycle(Graph *g);


#endif // !GRAPH_H