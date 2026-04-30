#include <stdio.h>
#include "linkedlist.h"

// Definisi pengganti NULL untuk arena
#define ARENA_NULL (size_t)-1

// Membuat Node baru di dalam arena
size_t list_create_node(Arena *a, int value) {
    // Minta ruang ke arena sebesar ukuran Node
    size_t node_offset = arena_alloc(a, sizeof(Node));
    
    // Ambil pointer aslinya sementara untuk mengisi data
    Node *new_node = (Node *)arena_get(a, node_offset);
    if (new_node != NULL) {
        new_node->data = value;
        new_node->next_offset = ARENA_NULL; // Penanda akhir list
    }
    
    // Kembalikan offset-nya (bukan pointernya)
    return node_offset;
}

// Menambahkan Node ke akhir Linked List
void list_append(Arena *a, size_t *head_offset, int value) {
    size_t new_node_offset = list_create_node(a, value);

    // Jika list masih kosong, node baru ini menjadi Head
    if (*head_offset == ARENA_NULL) {
        *head_offset = new_node_offset;
        return;
    }

    // Jika tidak kosong, lakukan traversal ke node terakhir
    size_t current_offset = *head_offset;
    Node *current_node = (Node *)arena_get(a, current_offset);

    while (current_node->next_offset != ARENA_NULL) {
        current_offset = current_node->next_offset;
        current_node = (Node *)arena_get(a, current_offset);
    }

    // Sambungkan node terakhir dengan node baru (menggunakan offset)
    current_node->next_offset = new_node_offset;
}

// Menelusuri dan mencetak isi Linked List
void list_traverse(Arena *a, size_t head_offset) {
    size_t current_offset = head_offset;
    printf("--- TRAVERSAL LINKED LIST ---\n");
    
    while (current_offset != ARENA_NULL) {
        Node *current_node = (Node *)arena_get(a, current_offset);
        
        // Output menampilkan Alamat (offset) dan Isi data sesuai syarat
        printf("Offset [%zu] | Data: %d -> ", current_offset, current_node->data);
        
        current_offset = current_node->next_offset;
    }
    printf("ARENA_NULL\n\n");
}