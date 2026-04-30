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

// Tambahkan fungsi ini di arena.c
void arena_dump(Arena *a) {
    printf("--- VISUALISASI MEMORI ARENA (ASCII 2D GRID) ---\n");
    printf("Kapasitas: %zu bytes | Terpakai: %zu bytes\n", a->capacity, a->curr_offset);
    
    // Menampilkan dalam bentuk grid (misal 16 byte per baris)
    int bytes_per_row = 16;
    
    for (size_t i = 0; i < a->capacity; i++) {
        // Cetak header offset di awal baris
        if (i % bytes_per_row == 0) {
            printf("[%04zu] ", i);
        }

        // Jika byte sudah terpakai (berada di belakang curr_offset)
        if (i < a->curr_offset) {
            // Cetak nilai heksadesimal dari byte tersebut
            printf("%02X ", a->buffer[i]); 
        } else {
            // Jika memori masih kosong / belum di-alokasi
            printf("-- "); 
        }

        // Pindah baris setiap 16 byte
        if ((i + 1) % bytes_per_row == 0) {
            printf("\n");
        }
    }
    printf("\n\n");
}