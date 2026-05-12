#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "arena.h"
#include <stddef.h>
#include <stdint.h> // TAMBAHKAN INI UNTUK uint32_t

#ifndef ARENA_NULL
#define ARENA_NULL (size_t)-1
#endif

typedef struct {
    uint32_t key;       // UBAH DARI int MENJADI uint32_t
    int frequency; 
    size_t next_offset; 
} HashNode;

typedef struct {
    size_t buckets_offset; 
    size_t num_buckets; 
} HashTable;

void hash_init(Arena *a, HashTable *ht, size_t num_buckets);

// UBAH PARAMETER key MENJADI uint32_t
int hash_record_stream(Arena *a, HashTable *ht, uint32_t key); 
void hash_print(Arena *a, HashTable *ht);

#endif