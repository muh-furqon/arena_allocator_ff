#ifndef ARENA_H
#define ARENA_H

#include <stddef.h> //untuk size_t
#include <stdint.h> //untuk tipe data dengan ukuran pasti seprti uint8_t

typedef struct {
    uint8_t *buffer; //Main memory block (memory yang akan dialokasi)
    size_t capacity; //Max capacity dari arena kita
    size_t curr_offset; //petunjuk offset memory yang sudah terpakai sekarang.
} Arena;

void arena_init(Arena *a, uint8_t *buffer, size_t size);
size_t arena_alloc(Arena *a, size_t size);
void *arena_get(Arena *a, size_t offset);
void arena_reset(Arena *a);
void arena_dump(Arena *a);

#endif