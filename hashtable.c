#include <stdio.h>
#include <stdint.h> // TAMBAHKAN INI
#include "hashtable.h"

// 1. UBAH PARAMETER DAN HAPUS CASTING
static size_t hash_function(uint32_t key, size_t num_buckets) {
    return key % num_buckets; // Langsung modulo karena sudah pasti positif
}

void hash_init(Arena *a, HashTable *ht, size_t num_buckets) {
    // ... (Isi fungsi ini TETAP SAMA persis) ...
    ht->num_buckets = num_buckets;
    ht->buckets_offset = arena_alloc(a, num_buckets * sizeof(size_t));
    size_t *buckets = (size_t *)arena_get(a, ht->buckets_offset);
    for (size_t i = 0; i < num_buckets; i++) buckets[i] = ARENA_NULL;
}

// 2. UBAH PARAMETER KEY
int hash_record_stream(Arena *a, HashTable *ht, uint32_t key) {
    // ... (Isi fungsi ini TETAP SAMA persis) ...
    size_t index = hash_function(key, ht->num_buckets);
    size_t *buckets = (size_t *)arena_get(a, ht->buckets_offset);
    size_t current_offset = buckets[index];
    
    while (current_offset != ARENA_NULL) {
        HashNode *node = (HashNode *)arena_get(a, current_offset);
        if (node->key == key) {
            // OPSI CAPPING: Jangan ditambah lagi jika sudah mencapai batas maksimum (misal: 1 Juta)
            // Ini mencegah Integer Overflow yang bisa mereset frekuensi ke angka negatif!
            if (node->frequency < 1000000) {
                node->frequency++;
            }
            return node->frequency;
        }
        current_offset = node->next_offset;
    }
    
    size_t new_node_offset = arena_alloc(a, sizeof(HashNode));
    HashNode *new_node = (HashNode *)arena_get(a, new_node_offset);
    if (new_node != NULL) {
        new_node->key = key;
        new_node->frequency = 1; 
        new_node->next_offset = buckets[index];
        buckets[index] = new_node_offset;
        return 1;
    }
    return -1; 
}

void hash_print(Arena *a, HashTable *ht) {
    size_t *buckets = (size_t *)arena_get(a, ht->buckets_offset);
    
    printf("=== REKAPITULASI FREKUENSI (FORMATTED IP) ===\n");
    for (size_t i = 0; i < ht->num_buckets; i++) {
        printf("Ember [%02zu]: ", i);
        
        size_t current_offset = buckets[i];
        if (current_offset == ARENA_NULL) {
            printf("-- Kosong --\n");
            continue;
        }
        
        while (current_offset != ARENA_NULL) {
            HashNode *node = (HashNode *)arena_get(a, current_offset);
            
            // ========================================================
            // REVERSE MAGIC: Mengurai uint32_t kembali ke 4 Octet
            // ========================================================
            uint32_t ip = node->key;
            unsigned char o1 = (ip >> 24) & 0xFF; // Geser 24 bit ke kanan, ambil 8 bit terakhir
            unsigned char o2 = (ip >> 16) & 0xFF; // Geser 16 bit ke kanan, ambil 8 bit terakhir
            unsigned char o3 = (ip >> 8)  & 0xFF; // Geser 8 bit ke kanan, ambil 8 bit terakhir
            unsigned char o4 = ip & 0xFF;         // Ambil 8 bit terakhir langsung
            
            printf("[%u.%u.%u.%u | Freq: %d] -> ", o1, o2, o3, o4, node->frequency);
            current_offset = node->next_offset;
        }
        printf("ARENA_NULL\n");
    }
    printf("=============================================\n\n");
}