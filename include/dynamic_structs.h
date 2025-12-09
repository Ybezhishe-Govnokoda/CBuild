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
} RawRules;

typedef enum {
	DS_OK = 0,
	DS_ERR_NOMEM = -1,
	DS_ERR_EOF = -2
} d_string_status;

typedef enum {
   RA_OK = 0,
   RA_ERR_NOMEM = -1,
} rules_array_status;

// Rule functions
void rule_init(Rule *r);

// Dynamic string functions
inline void ds_init(DString *s);
inline void ds_free(DString *s);
void ds_append_char(DString *s, char c);
void ds_append_str(DString *s, const char *str);
// Reads a line from file into DString s. Returns 1 on success, 0 on EOF.
short ds_readline(DString *s, FILE *fp);

// Dynamic array functions
void ra_init(RawRules *arr);
void ra_free(RawRules *arr);
short ra_append(RawRules *arr, Rule rule);

#endif // !DYNAMIC_STRUCTS_H