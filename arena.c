#include "arena.h"
#include <stdio.h> 

void arena_init(Arena *a, uint8_t *buffer_memory, size_t capacity) {
    a->buffer = buffer_memory;
    a->capacity = capacity;
    a->curr_offset = 0; // Mulai dari titik nol
}

size_t arena_alloc(Arena *a, size_t size) {
    // Validasi: Apakah sisa lapangan masih muat untuk data baru?
    if (a->curr_offset + size > a->capacity) {
        fprintf(stderr, "ERROR: Arena Out of Memory!\n");
        return (size_t)-1; // Mengembalikan nilai maksimum sebagai penanda error
    }

    // Simpan offset saat ini untuk dikembalikan kepada peminta memori
    size_t allocated_offset = a->curr_offset;
    
    // Geser garis batas (bump) sejauh ukuran yang diminta
    a->curr_offset += size; 

    return allocated_offset;
}

void *arena_get(Arena *a, size_t offset) {
    // Validasi keamanan tambahan
    if (offset >= a->capacity) {
        return NULL;
    }
    
    // Mengembalikan pointer asli dengan menjumlahkan alamat awal dan offset
    return (void *)(a->buffer + offset);
}

void arena_reset(Arena *a) {
    // Efek reset O(1): Cukup kembalikan offset ke 0.
    a->curr_offset = 0;
}