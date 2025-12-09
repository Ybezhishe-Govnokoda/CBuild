#include "hash_table.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define LOAD_FACTOR 0.7

// --------------- Hash function (FNV-1a) -----------------

// FNV-1a 64-bit hash, very stable for strings
static uint64_t hash_str(const char *s) {
   uint64_t h = 1469598103934665603ULL;
   while (*s) {
      h ^= (unsigned char)*s++;
      h *= 1099511628211ULL;
   }
   return h;
}

// ------------------ Utilities ----------------------------

// Linear probing: find index for a key or next free slot
static size_t probe_index(HashTable *ht, const char *key) {
   uint64_t h = hash_str(key);
   size_t idx = h % ht->capacity;

   while (ht->entries[idx].in_use &&
      strcmp(ht->entries[idx].key, key) != 0) {
      idx = (idx + 1) % ht->capacity;
   }
   return idx;
}

// Resize table when load factor becomes too high
static void hash_resize(HashTable *ht) {
   size_t new_cap = ht->capacity * 2;
   HashEntry *old_entries = ht->entries;
   size_t old_cap = ht->capacity;

   ht->entries = calloc(new_cap, sizeof(HashEntry));
   ht->capacity = new_cap;
   ht->size = 0;

   for (size_t i = 0; i < old_cap; i++) {
      if (old_entries[i].in_use) {
         // Reinsert
         const char *k = old_entries[i].key;
         struct Vertex *v = old_entries[i].val;
         hash_insert(ht, k, v);
      }
   }

   free(old_entries);
}

// ---------------- Public API -----------------------------

void hash_init(HashTable *ht, size_t initial_cap) {
   ht->entries = calloc(initial_cap, sizeof(HashEntry));
   ht->capacity = initial_cap;
   ht->size = 0;
}

void hash_free(HashTable *ht) {
   for (size_t i = 0; i < ht->capacity; i++) {
      if (ht->entries[i].in_use) {
         free(ht->entries[i].key);
      }
   }
   free(ht->entries);
}

void hash_insert(HashTable *ht, const char *key, struct Vertex *value) {
   // Resize if too full
   if ((double)ht->size / ht->capacity >= LOAD_FACTOR) {
      hash_resize(ht);
   }

   size_t idx = probe_index(ht, key);

   // New key?
   if (!ht->entries[idx].in_use) {
      ht->entries[idx].key = _strdup(key);
      ht->entries[idx].val = value;
      ht->entries[idx].in_use = OCCUPIED;
      ht->size++;
   }
   else {
      // Update existing
      ht->entries[idx].val = value;
   }
}

Vertex *hash_get(HashTable *ht, const char *key) {
   size_t idx = probe_index(ht, key);

   if (ht->entries[idx].in_use)
      return ht->entries[idx].val;

   return NULL;
}