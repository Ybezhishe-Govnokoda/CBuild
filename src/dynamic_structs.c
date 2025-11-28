#include "../include/dynamic_structs.h"


// Rule functions
void rule_init(Rule *r) {
   r->target = NULL;
   r->deps = NULL;
   r->commands = NULL;
   r->deps_count = 0;
   r->cmd_count = 0;
}


// Dynamic String functions
inline void ds_init(DString *s) {
   s->len = 0;
   s->cap = 16;
   s->data = malloc(s->cap);
   if (s->data) s->data[0] = '\0';
}

inline void ds_free(DString *s) {
   free(s->data);
   s->data = NULL;
   s->len = s->cap = 0;
}

static void ds_grow(DString *s, size_t new_cap) {
   if (!(s->data = realloc(s->data, new_cap))) {
      free(s->data);
      fprintf(stderr, "Error realloc for ds_grow\n");
      exit(1);
   }
   s->cap = new_cap;
}

void ds_append_char(DString *s, char c) {
   if (s->len + 2 > s->cap)
      ds_grow(s, s->cap * 2);

   s->data[s->len++] = c;
   s->data[s->len] = '\0';
}

void ds_append_str(DString *s, const char *str) {
   size_t add = strlen(str);
   if (s->len + add + 1 > s->cap)
      ds_grow(s, (s->len + add + 1) * 2);

   memcpy(s->data + s->len, str, add + 1);
   s->len += add;
}

int ds_readline(DString *s, FILE *fp) {
   ds_free(s);
   ds_init(s);

   int c;

   while ((c = fgetc(fp)) != EOF) {
      ds_append_char(s, (char)c);

      if (c == '\n') break;
   }

   // EOF with no data at all
   if (c == EOF && s->len == 0)
      return 0;

   return 1;
}


// RulesArray functions
void ra_init(RulesArray *arr) {
   arr->data = NULL;
   arr->count = 0;
   arr->capacity = 0;
}

void ra_free(RulesArray *arr) {
   if (!arr) return;

   for (size_t i = 0; i < arr->count; ++i) {
      free(arr->data[i].target);

      for (int d = 0; d < arr->data[i].deps_count; d++)
         free(arr->data[i].deps[d]);
      free(arr->data[i].deps);

      for (int c = 0; c < arr->data[i].cmd_count; c++)
         free(arr->data[i].commands[c]);
      free(arr->data[i].commands);
   }

   free(arr->data);
   arr->data = NULL;
   arr->count = 0;
   arr->capacity = 0;
}

void ra_append(RulesArray *arr, Rule rule) {
   if (arr->capacity == 0) {
      arr->capacity = sizeof(Rule);
      arr->data = malloc(arr->capacity);
      if (!arr->data) {
         perror("malloc");
         exit(1);
      }
   }

   if ((arr->count + 1) * sizeof(Rule) > arr->capacity) {
      arr->capacity *= 2;
      arr->data = realloc(arr->data, arr->capacity);
      if (!arr->data) {
         perror("realloc");
         exit(1);
      }
   }

   arr->data[arr->count++] = rule;
}


