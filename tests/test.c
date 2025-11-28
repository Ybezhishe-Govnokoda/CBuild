#include "../include/dynamic_structs.h"
#include "../include/file_parser.h"

int main()
{
   RulesArray arr;
   ra_init(&arr);

   if (parse_file("Buildfile", &arr) != PARSE_OK) {
      fprintf(stderr, "Error parsing file\n");
      return 1;
   }

   for (size_t i = 0; i < arr.count; i++) {
      Rule *r = &arr.data[i];

      printf("TARGET: %s\n", r->target);
      for (int d = 0; d < r->deps_count; d++)
         printf("  DEP: %s\n", r->deps[d]);
      for (int c = 0; c < r->cmd_count; c++)
         printf("  CMD: %s\n", r->commands[c]);
      printf("\n");
   }

   ra_free(&arr);
}

