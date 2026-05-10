#include <stdio.h>
#include "hashtable.h"

// Fungsi Hash Sederhana (Modulo)
// Mengubah kunci/ID menjadi indeks ember (bucket)
static size_t hash_function(int key, size_t num_buckets) {
    // Memastikan kunci negatif tetap aman (jika ada)
    unsigned int u_key = (unsigned int)key;
    return u_key % num_buckets;
}

// 1. Inisialisasi Hash Table
void hash_init(Arena *a, HashTable *ht, size_t num_buckets) {
    ht->num_buckets = num_buckets;
    
    // Minta arena membuatkan Array yang berisi kumpulan offset
    ht->buckets_offset = arena_alloc(a, num_buckets * sizeof(size_t));
    
    // Dapatkan pointer asli ke Array tersebut untuk mengisinya
    size_t *buckets = (size_t *)arena_get(a, ht->buckets_offset);
    
    // Kosongkan semua ember (isi dengan ARENA_NULL)
    for (size_t i = 0; i < num_buckets; i++) {
        buckets[i] = ARENA_NULL;
    }
}

// 2. Mencatat Data Stream (Mencari dan Memperbarui/Menambah)
int hash_record_stream(Arena *a, HashTable *ht, int key) {
    size_t index = hash_function(key, ht->num_buckets);
    size_t *buckets = (size_t *)arena_get(a, ht->buckets_offset);
    size_t current_offset = buckets[index];
    
    // FASE 1: Cari apakah IP ini sudah ada
    while (current_offset != ARENA_NULL) {
        HashNode *node = (HashNode *)arena_get(a, current_offset);
        if (node->key == key) {
            node->frequency++;
            return node->frequency; // KEMBALIKAN frekuensi terbaru
        }
        current_offset = node->next_offset;
    }
    
    // FASE 2: Jika IP baru
    size_t new_node_offset = arena_alloc(a, sizeof(HashNode));
    HashNode *new_node = (HashNode *)arena_get(a, new_node_offset);
    if (new_node != NULL) {
        new_node->key = key;
        new_node->frequency = 1; 
        new_node->next_offset = buckets[index];
        buckets[index] = new_node_offset;
        return 1; // KEMBALIKAN 1 karena ini kemunculan pertama
    }
    return 0; // Jika gagal
}

// 3. Mencetak Isi Hash Table (Rekapitulasi)
void hash_print(Arena *a, HashTable *ht) {
    size_t *buckets = (size_t *)arena_get(a, ht->buckets_offset);
    
    printf("=== REKAPITULASI FREKUENSI (HASH TABLE) ===\n");
    for (size_t i = 0; i < ht->num_buckets; i++) {
        printf("Ember [%02zu]: ", i);
        
        size_t current_offset = buckets[i];
        if (current_offset == ARENA_NULL) {
            printf("-- Kosong --\n");
            continue;
        }
        
        // Jika ada isinya, telusuri rantainya
        while (current_offset != ARENA_NULL) {
            HashNode *node = (HashNode *)arena_get(a, current_offset);
            printf("[ID: %d | Freq: %d] -> ", node->key, node->frequency);
            current_offset = node->next_offset;
        }
        printf("ARENA_NULL\n");
    }
    printf("===========================================\n\n");
}