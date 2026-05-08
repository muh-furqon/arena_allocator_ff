#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "arena.h"
#include <stddef.h>

#ifndef ARENA_NULL
#define ARENA_NULL (size_t)-1
#endif

typedef struct {
    int key; // ID data stream (ex: kode/nomor punggung pemain)
    int frequency; // Jumlah kemunculan (dimulai dari 1)
    size_t next_offset; // Offset ke node selanjutnya kalau ada collision
} HashNode;

typedef struct {
    size_t buckets_offset; // offset menuju array of size_t (save head tiap rantai)
    size_t num_buckets; // jumlah / total slot / bucket tersedia
} HashTable;

void hash_init(Arena *a, HashTable *ht, size_t num_buckets);
void hash_record_stream(Arena *a, HashTable *ht, int key);
void hash_print(Arena *a, HashTable *ht);

#endif

