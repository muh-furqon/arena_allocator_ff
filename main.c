#include <stdio.h>
#include "arena.h"

#define ARENA_CAPACITY 1024 // 1kb

//Static buffer menggantikan malloc
uint8_t static_buffer[ARENA_CAPACITY];

int main() {
    Arena my_arena;

    //Inisiasi
    arena_init(&my_arena, static_buffer, ARENA_CAPACITY);
    printf("--- STATUS ARENA AWAL ---\n");
    printf("Kapasitas: %zu bytes\n", my_arena.capacity);
    printf("Offset awal: %zu\n\n", my_arena.curr_offset);

    // Simulasi alokasi
    // Anggap 1 integer butuh 4 bytes
    printf("--- MELAKUKAN ALOKASI ---\n");
    size_t offset_data1 = arena_alloc(&my_arena, sizeof(int));
    size_t offset_data2 = arena_alloc(&my_arena, sizeof(int));

    printf("Data 1 dialokasikan pada offset: %zu\n", offset_data1);
    printf("Data 2 dialokasikan pada offset: %zu\n", offset_data2);
    printf("Batas offset arena sekarang: %zu\n\n", my_arena.curr_offset);

    // 3. Menulis dan Membaca Data menggunakan arena_get
    int *ptr_data1 = (int *)arena_get(&my_arena, offset_data1);
    *ptr_data1 = 1999; // Menyimpan nilai

    int *ptr_data2 = (int *)arena_get(&my_arena, offset_data2);
    *ptr_data2 = 2026; // Menyimpan nilai

    printf("--- MEMBACA DATA ---\n");
    printf("Nilai di offset %zu: %d\n", offset_data1, *ptr_data1);
    printf("Nilai di offset %zu: %d\n\n", offset_data2, *ptr_data2);

    // 4. Simulasi Reset O(1)
    printf("--- MELAKUKAN RESET ---\n");
    arena_reset(&my_arena);
    printf("Batas offset arena setelah reset: %zu\n", my_arena.curr_offset);

    return 0;
}