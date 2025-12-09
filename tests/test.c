#include "../include/dynamic_structs.h"
#include "../include/file_parser.h"
#include "../include/graph.h"

//#define DEBUG

int main()
{
   RawRules arr;
   ra_init(&arr);

   if (parse_file("Buildfile", &arr) != PARSE_OK) {
      fprintf(stderr, "Error parsing file\n");
      return 1;
   }

#ifdef DEBUG
   printf("parse_file returned ok; arr.count = %zu, arr.capacity = %zu\n",
      arr.count, arr.capacity);
   fflush(stdout);
#endif

   for (size_t i = 0; i < arr.count; i++) {
      Rule *r = &arr.data[i];

      printf("TARGET: %s\n", r->target);
      for (int d = 0; d < r->deps_count; d++)
         printf("  DEP: %s\n", r->deps[d]);
      for (int c = 0; c < r->cmd_count; c++)
         printf("  CMD: %s\n", r->commands[c]);
      printf("\n");
   }


   Graph graph;
   graph_init(&graph);

#ifdef DEBUG
   printf("graph.count after init = %d\n", graph.count);
   fflush(stdout);
#endif

   /* После того как у тебя есть RulesArray arr (все правила) */
   for (int i = 0; i < arr.count; ++i) {
      Rule *r = &arr.data[i];
      // создаём вершину для цели (если не создавалась)
      // но graph_add_edge создаст их сам. Здесь удобно связать vertex->rule.
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

#ifdef DEBUG
   printf("graph.count before topo = %d\n", graph.count);
   fflush(stdout);
#endif

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

   for (int i = 0; i < graph.count; i++)
   {
      printf_s("Vertex %d Name: %s\n", i, graph.vertices[i]->name);
      printf_s("Vertex %d Rule's target: %s\n", i, graph.vertices[i]->rule->target);

      for (int d = 0; d < graph.vertices[i]->rule->deps_count; d++)
         printf("  DEP: %s\n", graph.vertices[i]->rule->deps[d]);
      for (int c = 0; c < graph.vertices[i]->rule->cmd_count; c++)
         printf("  CMD: %s\n", graph.vertices[i]->rule->commands[c]);
      printf("\n");

		for (int j = 0; j < graph.vertices[i]->dep_count; j++)
         printf_s("    Dep %d: %s\n", j, graph.vertices[i]->deps[j]->name);
   }

   ra_free(&arr);
	graph_free(&graph);
}