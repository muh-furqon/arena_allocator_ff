#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "arena.h"
#include <stddef.h>

// Pindahkan definisi ke sini agar bisa dibaca oleh main.c
#define ARENA_NULL (size_t)-1 

// Struktur Node berbasis offset
typedef struct {
    int data;
    size_t next_offset;
} Node;

// Prototipe fungsi Linked List
size_t list_create_node(Arena *a, int value);
void list_append(Arena *a, size_t *head_offset, int value);
void list_traverse(Arena *a, size_t head_offset);

#endif