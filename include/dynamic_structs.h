#ifndef DYNAMIC_STRUCTS_H
#define DYNAMIC_STRUCTS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
   char *target;
   char **deps;
   int deps_count;
   char **commands;
   int cmd_count;
} Rule;

typedef struct {
   char *data; 
   size_t len;
   size_t cap;
} DString;

typedef struct {
   Rule *data;
   size_t count;
   size_t capacity;
}RulesArray;

// Rule functions
void rule_init(Rule *r);

// Dynamic string functions
inline void ds_init(DString *s);
inline void ds_free(DString *s);
void ds_append_char(DString *s, char c);
void ds_append_str(DString *s, const char *str);
int ds_readline(DString *s, FILE *fp);

// Dynamic array functions
void ra_init(RulesArray *arr);
void ra_free(RulesArray *arr);
void ra_append(RulesArray *arr, Rule rule);

#endif

