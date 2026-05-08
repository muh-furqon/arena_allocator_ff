#include <stdio.h>
#include "arena.h"
#include "hashtable.h"

// Kapasitas sangat sempit (256 bytes) untuk memaksa collision dan memudahkan screenshot
#define ARENA_CAPACITY 256 

uint8_t static_buffer[ARENA_CAPACITY];

int main() {
    Arena my_arena;
    arena_init(&my_arena, static_buffer, ARENA_CAPACITY);

    printf("=== MINGGU 3: EKSPERIMEN DATA STREAM ===\n");
    printf("Studi Kasus: Real-Time IP Address Tracker (DDoS Detection)\n\n");

    HashTable ip_tracker;
    // Server kita disimulasikan punya keterbatasan memori ekstrem, 
    // hanya ada 5 bucket untuk menampung rentang jutaan kemungkinan IP!
    hash_init(&my_arena, &ip_tracker, 5); 
    printf("[*] Sistem Hash Table aktif dengan 5 buckets.\n");
    printf("[*] Memantau aliran paket jaringan masuk...\n\n");

    // Representasi IP Address dalam bentuk Integer 32-bit
    // Menggunakan angka acak besar untuk menyimulasikan alamat IP asli
    int ip_attacker  = 19216801; // Mewakili IP Penyerang (Spammer)
    int ip_normal_1  = 10000005; // Mewakili IP Normal A
    int ip_normal_2  = 17201601; // Mewakili IP Normal B
    int ip_attacker2 = 80808080; // Mewakili IP Penyerang Kedua

    // Aliran paket (request) yang memborbardir server dalam 1 detik
    int packet_stream[] = {
        ip_normal_1, ip_attacker, ip_attacker, ip_normal_2, 
        ip_attacker, ip_attacker2, ip_attacker, ip_normal_1,
        ip_attacker, ip_attacker, ip_attacker2, ip_attacker
    };
    
    int total_packets = sizeof(packet_stream) / sizeof(packet_stream[0]);

    // Memproses aliran data secara O(1)
    for (int i = 0; i < total_packets; i++) {
        hash_record_stream(&my_arena, &ip_tracker, packet_stream[i]);
    }
    
    printf("[!] Peringatan: %d paket telah diproses secara real-time!\n", total_packets);
    printf("[!] Peringatan: Aktivitas anomali terdeteksi dari beberapa IP!\n\n");

    // 1. Menampilkan Hasil Deteksi (Pembuktian Collision dan Linked List)
    hash_print(&my_arena, &ip_tracker);

    // 2. Bukti Penggunaan Memori (Syarat Mutlak Laporan)
    printf("=== BUKTI PENGGUNAAN MEMORI (ARENA DUMP) ===\n");
    arena_dump(&my_arena);

    arena_reset(&my_arena);
    arena_dump(&my_arena);

    return 0;
}