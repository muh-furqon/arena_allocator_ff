#include "arena.h"
#include <stdio.h>

void arena_init(Arena *a, uint8_t *buffer, size_t capacity) {
    a->buffer = buffer;
    a->capacity = capacity;
    a->curr_offset = 0; //Mulai dari 0 ya
}

size_t arena_alloc(Arena *a, size_t size) {
    // Validation: Apakah arena masih bisa menampung data baru?
    if (a->curr_offset + size > a->capacity) {
        fprintf(stderr, "ERROR: Arena Overflow!");
        return(size_t)-1; // Return nilai maksimum sebagai penanda error
    }

    //Simpan offset sekarang untuk dikembalikan ke memory request
    size_t allocated_offset = a->curr_offset;

    // Geser garis batas (bump) sejauh ukuran yang direquest
    a->curr_offset += size;

    return allocated_offset;
}

void *arena_get(Arena *a, size_t offset) {
    // Validation keamanan tambahan
    if (offset >= a->curr_offset) {
        return NULL;
    }

    // Return pointer asli dengan menjumlahkan alamat awal dan offset
    return (void *)(a->buffer + offset);
}

void arena_reset(Arena *a) {
    // Reset O(1): Reset offset ke 0
    a->curr_offset = 0;
}