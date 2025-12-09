#define DEBUG
#define MAIN_TEST
//#define HASH_TABLE_TEST

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
#ifdef MAIN_TEST

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

#endif // MAIN_TEST

#ifdef HASH_TABLE_TEST
	Vertex test1 = { .name = "file1.o", .rule = NULL, .deps = NULL, .dep_count = 0, .dep_cap = 0, .color = WHITE };
	Vertex test2 = { .name = "file2.o", .rule = NULL, .deps = NULL, .dep_count = 0, .dep_cap = 0, .color = WHITE };
	Vertex test3 = { .name = "main.o", .rule = NULL, .deps = NULL, .dep_count = 0, .dep_cap = 0, .color = WHITE };

   HashTable ht;
   hash_init(&ht, 8);
   // Insert some entries
   hash_insert(&ht, test1.name, &test1);
   hash_insert(&ht, test2.name, &test2);
   hash_insert(&ht, test3.name, &test3);
   // Retrieve entries
   Vertex *v1 = hash_get(&ht, "file1.o");
   Vertex *v2 = hash_get(&ht, "file2.o");
   Vertex *v3 = hash_get(&ht, "main.o");
   Vertex *v4 = hash_get(&ht, "nonexistent.o");
   printf("Retrieved: %p %p %p %p\n", (void*)v1, (void*)v2, (void*)v3, (void*)v4);
	hash_free(&ht);
#endif // HASH_TABLE_TEST

	return 0;
}