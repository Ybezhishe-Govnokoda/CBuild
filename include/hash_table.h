#ifndef HASH_H
#define HASH_H

#include <stddef.h>

// Forward declaration
struct Vertex;
typedef struct Vertex Vertex;

typedef struct {
   char *key;              // File name (target/dependency)
   Vertex *val;            // Pointer to graph node
   int in_use;             // Flag: 0 = empty, 1 = occupied
} HashEntry;

typedef struct {
   HashEntry *entries;     // Array of entries
   size_t capacity;        // Total slots
   size_t size;            // Number of used slots
} HashTable;

typedef enum {
	EMPTY,
	OCCUPIED,
};

// Initialize table with given capacity
void hash_init(HashTable *ht, size_t initial_cap);

// Free all memory inside hash table
void hash_free(HashTable *ht);

// Insert or update existing key
void hash_insert(HashTable *ht, const char *key, Vertex *value);

// Get value by key, return NULL if not found
Vertex *hash_get(HashTable *ht, const char *key);

#endif // HASH_H