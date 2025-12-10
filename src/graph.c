#include "graph.h"


// Auxiliary
static Vertex *vertex_create(const char *name) {
   Vertex *v = malloc(sizeof(Vertex));
   if (!v) return NULL;
   v->name = _strdup(name);
   if (!v->name) { free(v); return NULL; }
   v->rule = NULL;
   v->deps = NULL;
   v->dep_count = 0;
   v->dep_cap = 0;
   v->color = 0;
   return v;
}

static void vertex_free(Vertex *v) {
   if (!v) return;
   free(v->name);
	free(v->deps); // Don't free Rule* - managed in parser
   free(v);
}

void graph_init(Graph *g) {
   g->vertices = NULL;
   g->count = 0;
   g->cap = 0;
   hash_init(&g->map, 64);
}

void graph_free(Graph *g) {
   if (!g) return;
   for (int i = 0; i < g->count; ++i) 
      vertex_free(g->vertices[i]);

   free(g->vertices);
   g->vertices = NULL;
   g->count = g->cap = 0;

	hash_free(&g->map);
}

static int graph_reserve(Graph *g, int min_cap) {
   if (g->cap >= min_cap) return 1;

   int newcap = g->cap ? g->cap * 2 : 8;
   while (newcap < min_cap) 
      newcap *= 2;

   Vertex **nv = realloc(g->vertices, sizeof(Vertex *) * newcap);
   if (!nv) return 0;

   g->vertices = nv;
   g->cap = newcap;
   return 1;
}

inline Vertex *graph_find(const Graph *g, const char *name) {
   for (int i = 0; i < g->count; ++i) {
      if (strcmp(g->vertices[i]->name, name) == 0) 
         return g->vertices[i];
   }
   return NULL;
}

Vertex *graph_find_or_create(Graph *g, const char *name, int *err) {
   // Try to find in hash table
   Vertex *v = hash_get(&g->map, name);
   if (v) return v;

	// Create new vertex if not found
   v = vertex_create(name);
   if (!v) {
      if (err) *err = 1;
      return NULL;
   }

   // Add to dynamic array of all vertices
   if (g->count == g->cap) {
      int newcap = g->cap ? g->cap * 2 : 16;
      Vertex **nv = realloc(g->vertices, newcap * sizeof(Vertex *));
      if (!nv) {
         free(v);
         if (err) *err = 1;
         return NULL;
      }
      g->vertices = nv;
      g->cap = newcap;
   }

   g->vertices[g->count++] = v;

   // Add to hash table
   hash_insert(&g->map, name, v);

   return v;
}


int graph_add_edge(Graph *g, const char *from_name, const char *to_name) {
   int err = 0;
   Vertex *from = graph_find_or_create(g, from_name, &err);
   if (!from) return GRAPH_ERR_NOMEM;

   Vertex *to = graph_find_or_create(g, to_name, &err);
   if (!to) return GRAPH_ERR_NOMEM;

   // Add "to" to the list of dependencies from->deps
   if (from->dep_count + 1 > from->dep_cap) {
      int newcap = from->dep_cap ? from->dep_cap * 2 : 4;
      Vertex **nv = realloc(from->deps, sizeof(Vertex *) * newcap);
      if (!nv) return GRAPH_ERR_NOMEM;
      from->deps = nv;
      from->dep_cap = newcap;
   }
   from->deps[from->dep_count++] = to;
   return GRAPH_OK;
}


/* DFS for cycle detection and topological sort */
static int dfs_visit(Vertex *v, Vertex **stack, int *stack_pos) {
   if (v->color == GRAY)
      return GRAPH_ALREADY_IN_STACK; // Already in stack (shouldn't happen if called properly)
   
   v->color = GRAY;

   for (int i = 0; i < v->dep_count; ++i) {
      Vertex *w = v->deps[i];
      if (w->color == WHITE) {
         int ok = dfs_visit(w, stack, stack_pos);
         if (ok != GRAPH_OK) return GRAPH_ERR_CYCLE;
      }
      else if (w->color == GRAY) {
         // Find back-edge => cycle
         return GRAPH_ERR_CYCLE;
      }
   }

   v->color = BLACK;
   stack[(*stack_pos)++] = v; // postorder push
   return GRAPH_OK;
}

int graph_topo_sort(Graph *g, Vertex ***out_order, int *out_n) {
   if (!g) return GRAPH_ERR_NOMEM;
   // reset colors
   for (int i = 0; i < g->count; ++i) g->vertices[i]->color = WHITE;

   Vertex **stack = malloc(sizeof(Vertex *) * g->count);
   if (!stack) return GRAPH_ERR_NOMEM;
   int sp = 0;

   for (int i = 0; i < g->count; ++i) {
      if (g->vertices[i]->color == WHITE) {
         int ok = dfs_visit(g->vertices[i], stack, &sp);
         if (ok != GRAPH_OK) { 
            free(stack); 
            return GRAPH_ERR_CYCLE; 
         }
      }
   }

   // stack now contains vertices in reverse topological order (postorder)
   // we need to reverse it to get correct build order
   Vertex **order = malloc(sizeof(Vertex *) * g->count);
   if (!order) { 
      free(stack); 
      return GRAPH_ERR_NOMEM; 
   }
   for (int i = 0; i < g->count; ++i) order[i] = stack[g->count - 1 - i];

   free(stack);
   *out_order = order;
   *out_n = g->count;
   return GRAPH_OK;
}