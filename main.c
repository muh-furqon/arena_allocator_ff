#include <stdio.h>
#include "arena.h"
#include "linkedlist.h" // Wajib di-include agar Linked List terbaca

#define ARENA_CAPACITY 512 // 1 KB

uint8_t static_buffer[ARENA_CAPACITY];

int main() {
    Arena my_arena;
    arena_init(&my_arena, static_buffer, ARENA_CAPACITY);

    printf("=== MINGGU 2: PENGUJIAN STRUKTUR DATA ===\n\n");

    // ---------------------------------------------------------
    // 1. ARRAY DALAM ARENA (KONTIGU)
    // ---------------------------------------------------------
    printf("[1] ALOKASI ARRAY (5 Elemen Integer)\n");
    // Alokasi memori untuk 5 buah integer sekaligus
    size_t array_offset = arena_alloc(&my_arena, 5 * sizeof(int));
    
    // Dapatkan pointer asli ke awal array
    int *arr = (int *)arena_get(&my_arena, array_offset);
    
    // Isi array dan cetak alamatnya
    for (int i = 0; i < 5; i++) {
        arr[i] = (i + 1) * 10; // Mengisi dengan nilai 10, 20, 30, 40, 50
        size_t current_item_offset = array_offset + (i * sizeof(int));
        printf("Array[%d] di offset [%zu] | Nilai: %d\n", i, current_item_offset, arr[i]);
    }
    printf("\n");

    // ---------------------------------------------------------
    // 2. LINKED LIST BERBASIS OFFSET
    // ---------------------------------------------------------
    printf("[2] ALOKASI LINKED LIST\n");
    size_t head = ARENA_NULL; // Inisialisasi list kosong
    
    // Tambahkan beberapa node
    list_append(&my_arena, &head, 1999);
    list_append(&my_arena, &head, 2026);
    list_append(&my_arena, &head, 9999);

    // Lakukan traversal untuk mencetak isinya
    list_traverse(&my_arena, head);

    // ---------------------------------------------------------
    // 3. VISUALISASI MEMORI (ARENA DUMP)
    // ---------------------------------------------------------
    printf("[3] KONDISI MEMORI SETELAH ALOKASI\n");
    arena_dump(&my_arena);

    return 0;
}