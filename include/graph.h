#ifndef GRAPH_H
#define GRAPH_H

#include "utils.h" // For Rule
#include "hash_table.h"
#include <stdbool.h>


typedef struct Vertex {
   char *name;
   Rule *rule;

   struct Vertex **deps;
   int dep_count;
   int dep_cap;

   // 0 = white, 1 = gray, 2 = black
   int color;

	// Caching and build info for build engine
   bool checked;
   bool outdated;
   bool built;
} Vertex;

typedef struct {
   // Array of all vertices
	Vertex **vertices;
   int count;
   int cap;

   // name -> Vertex*
   HashTable map;
} Graph;


typedef enum { 
   GRAPH_OK = 0, 
   GRAPH_ERR_NOMEM = -1, 
   GRAPH_ERR_CYCLE = -2,
	GRAPH_ALREADY_IN_STACK = -3,
	GRAPH_ERR_UNKNOWN = -4
} graph_status;

typedef enum {
   WHITE,
   GRAY,
   BLACK
} vertex_color;

void graph_init(Graph *g);
void graph_free(Graph *g);

/* Adds edge (from -> to). If there are no vertices, creates them. */
int graph_add_edge(Graph *g, const char *from_name, const char *to_name);

// Find vertex by name or create it if not found. Return
Vertex *graph_find_or_create(Graph *g, const char *name, int *err);

/* Build a topological sort.
   If successful, returns the GRAPH_OK and fills in the out_order with the number of out_n.
   out_order is allocated via malloc - the caller must free(out_order)
   If there is a loop, returns GRAPH_ERR_CYCLE. */
int graph_topo_sort(Graph *g, Vertex ***out_order, int *out_n);

#endif // !GRAPH_H