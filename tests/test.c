//#define DEBUG

#include "../include/dynamic_structs.h"
#include "../include/file_parser.h"
#include "../include/graph.h"

#ifdef DEBUG
void graph_print(Graph *g) {
   if (!g) { printf("(null graph)\n"); return; }

   printf("===== GRAPH DUMP (count=%d) =====\n", g->count);
   for (int i = 0; i < g->count; ++i) {
      Vertex *v = g->vertices[i];
      printf("Vertex %d Name: '%s'\n", i, v->name ? v->name : "(null)");
      if (v->rule) {
         printf("  Rule target: '%s'\n", v->rule->target ? v->rule->target : "(null)");
         printf("  Rule deps_count=%d, cmd_count=%d\n", v->rule->deps_count, v->rule->cmd_count);
         for (int d = 0; d < v->rule->deps_count; ++d)
            printf("    rule.dep[%d] = '%s'\n", d, v->rule->deps[d] ? v->rule->deps[d] : "(null)");
         for (int c = 0; c < v->rule->cmd_count; ++c)
            printf("    rule.cmd[%d] = '%s'\n", c, v->rule->commands[c] ? v->rule->commands[c] : "(null)");
      }
      else {
         printf("  (no rule for this vertex — it's a file)\n");
      }

      printf("  adjacency (dep_count=%d): ", v->dep_count);
      if (v->dep_count == 0) { printf("<none>\n"); }
      else {
         for (int j = 0; j < v->dep_count; ++j) {
            printf("'%s'%s", v->deps[j]->name, j + 1 == v->dep_count ? "\n" : ", ");
         }
      }
      printf("\n");
   }
   printf("===== END GRAPH DUMP =====\n");
   fflush(stdout);
}
#endif


int main()
{
   RawRules arr;
   ra_init(&arr);

   if (parse_file("Buildfile", &arr) != PARSE_OK) {
      fprintf(stderr, "Error parsing file\n");
      return 1;
   }

#ifdef DEBUG
   for (size_t i = 0; i < arr.count; i++) {
      Rule *r = &arr.data[i];

      printf("TARGET: %s\n", r->target);
      for (int d = 0; d < r->deps_count; d++)
         printf("  DEP: %s\n", r->deps[d]);
      for (int c = 0; c < r->cmd_count; c++)
         printf("  CMD: %s\n", r->commands[c]);
      printf("\n");
   }
#endif // DEBUG

   Graph graph;
   graph_init(&graph);

   
   for (int i = 0; i < arr.count; ++i) {
      Rule *r = &arr.data[i];
      Vertex *v = graph_find_or_create(&graph, r->target, NULL);
      v->rule = r;
   }

   /* Теперь соединяем зависимости: from = rule.target, to = each dep name */
   for (int i = 0; i < arr.count; ++i) {
      Rule *r = &arr.data[i];
      for (int d = 0; d < r->deps_count; ++d) {
         graph_add_edge(&graph, r->target, r->deps[d]);
      }
   }

   /* Теперь можно: */
   Vertex **order;
   int n;
   if (graph_topo_sort(&graph, &order, &n) == GRAPH_OK) {
      // order[0] ... order[n-1] — порядок сборки (targets first -> deps later? см.ниже)
      // Ты можешь пройти order и решать, какие команды запускать
      free(order);
   }
   else {
      fprintf(stderr, "Cycle detected in dependency graph\n");
   }

#ifdef DEBUG
   graph_print(&graph); 
#endif // DEBUG

   ra_free(&arr);
	graph_free(&graph);
}