#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "arena.h"
#include <stddef.h>

// Struktur Node berbasis offset
typedef struct {
    int data;
    size_t next_offset; // Menyimpan jarak (offset), bukan pointer asli!
} Node;

// Prototipe fungsi Linked List
size_t list_create_node(Arena *a, int value);
void list_append(Arena *a, size_t *head_offset, int value);
void list_traverse(Arena *a, size_t head_offset);

#endif