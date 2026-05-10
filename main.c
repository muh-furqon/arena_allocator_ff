#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Library mutlak untuk benchmarking waktu
#include "arena.h"
#include "hashtable.h"

#define ARENA_CAPACITY 256
#define DDOS_THRESHOLD 5

uint8_t static_buffer[ARENA_CAPACITY];

int main() {
    Arena my_arena;
    arena_init(&my_arena, static_buffer, ARENA_CAPACITY);

    HashTable ip_tracker;
    hash_init(&my_arena, &ip_tracker, 5); 

    printf("====================================================\n");
    printf(" [ SERVER SECURITY DASHBOARD & BENCHMARK SUITE ]\n");
    printf("====================================================\n");
    printf(" PERINTAH TERSEDIA:\n");
    printf(" - Ketik angka IP (misal: 1921681) lalu Enter\n");
    printf(" - Ketik 'STATUS' untuk cek isi Hash Table\n");
    printf(" - Ketik 'DUMP'   untuk cek forensik memori Arena\n");
    printf(" - Ketik 'BENCHMARK' untuk uji performa vs Malloc\n");
    printf(" - Ketik 'EXIT'   untuk mematikan server\n");
    printf("====================================================\n\n");

    char input[64];

    while (1) {
        printf("Server_Port> ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "EXIT") == 0) {
            printf("[*] Mematikan server pemantau... Selesai.\n");
            break;
        } 
        else if (strcmp(input, "STATUS") == 0) {
            hash_print(&my_arena, &ip_tracker);
        } 
        else if (strcmp(input, "DUMP") == 0) {
            arena_dump(&my_arena);
        }
        // ===== FITUR BARU: MINGGU 4 BENCHMARKING =====
        else if (strcmp(input, "BENCHMARK") == 0) {
            int N = 500000; // Setengah juta node
            printf("\n[*] MEMULAI STRESS TEST & BENCHMARKING (%d Alokasi)...\n", N);
            
            clock_t start, end;
            double time_malloc_alloc, time_malloc_free;
            double time_arena_alloc, time_arena_reset;

            // --- 1. UJI COBA MALLOC BAWAAN OS ---
            // Siapkan array untuk menampung pointer agar bisa di-free nanti
            HashNode **malloc_ptrs = malloc(N * sizeof(HashNode *));
            
            start = clock();
            for(int i = 0; i < N; i++) {
                malloc_ptrs[i] = malloc(sizeof(HashNode));
                malloc_ptrs[i]->key = i; // Simulasi pengisian data
            }
            end = clock();
            time_malloc_alloc = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;

            start = clock();
            for(int i = 0; i < N; i++) {
                free(malloc_ptrs[i]); // Harus dihapus SATU PER SATU (O(n))
            }
            end = clock();
            time_malloc_free = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;
            free(malloc_ptrs);

            // --- 2. UJI COBA ARENA ALLOCATOR ---
            // Siapkan kapasitas arena besar khusus untuk benchmark (sekitar 8 MB)
            size_t bench_capacity = N * sizeof(HashNode) + 1024;
            uint8_t *bench_buffer = malloc(bench_capacity);
            Arena bench_arena;
            arena_init(&bench_arena, bench_buffer, bench_capacity);

            start = clock();
            for(int i = 0; i < N; i++) {
                size_t offset = arena_alloc(&bench_arena, sizeof(HashNode));
                HashNode *node = (HashNode *)arena_get(&bench_arena, offset);
                node->key = i; // Simulasi pengisian data
            }
            end = clock();
            time_arena_alloc = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;

            start = clock();
            // Reset arena (O(1)) -> Hanya mengubah satu angka saja!
            bench_arena.curr_offset = 0; 
            end = clock();
            time_arena_reset = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;

            free(bench_buffer); // Hapus arena benchmark

            // --- 3. CETAK HASIL LAPORAN ---
            printf("\n========================================================\n");
            printf(" HASIL BENCHMARK (Satuan: Milidetik / ms)\n");
            printf("========================================================\n");
            printf(" Operasi           | Standar (Malloc) | Arena Allocator\n");
            printf("-------------------|------------------|-----------------\n");
            printf(" Alokasi %6d x | %13.2f ms | %12.2f ms\n", N, time_malloc_alloc, time_arena_alloc);
            printf(" Free / Reset      | %13.2f ms | %12.2f ms\n", time_malloc_free, time_arena_reset);
            printf("========================================================\n");
            
            // Analisis otomatis untuk dipamerkan
            double alloc_speedup = time_malloc_alloc / (time_arena_alloc == 0 ? 0.001 : time_arena_alloc);
            printf(" [!] KESIMPULAN: Arena Allocator %.1fx LEBIH CEPAT saat alokasi!\n\n", alloc_speedup);
        }
        else if (strlen(input) == 0) { continue; }
        else {
            int ip_address = atoi(input);
            if (ip_address == 0 && strcmp(input, "0") != 0) {
                printf("[!] Perintah tidak dikenali.\n");
                continue;
            }

            int freq = hash_record_stream(&my_arena, &ip_tracker, ip_address);
            
            if (freq < DDOS_THRESHOLD) {
                printf("[+] Paket dari IP [%d] diterima. (Total request: %d)\n", ip_address, freq);
            } 
            else if (freq == DDOS_THRESHOLD) {
                printf("\n====================================================\n");
                printf(" [!!!] PERINGATAN ANOMALI KEAMANAN TERDETEKSI [!!!]\n");
                printf(" IP %d telah mengirim %d request beruntun!\n", ip_address, DDOS_THRESHOLD);
                printf(" Tindakan: IP Banned. Akses ditutup.\n");
                printf("====================================================\n\n");
            } 
            else {
                printf("[-] KONEKSI DITOLAK: IP [%d] berada dalam daftar BANNED! (Drop count: %d)\n", ip_address, freq - DDOS_THRESHOLD);
            }
        }
    }

    return 0;
}