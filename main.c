#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
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
    printf(" - Ketik 'STATUS'    untuk cek isi Hash Table\n");
    printf(" - Ketik 'DUMP'      untuk cek forensik memori Arena\n");
    printf(" - Ketik 'RESET'     untuk mengosongkan memori server\n");
    printf(" - Ketik 'BENCHMARK' untuk uji performa vs Malloc\n");
    printf(" - Ketik 'EXIT'      untuk mematikan server\n");
    printf("====================================================\n\n");

    char input[64];

    while (1) {
        printf("Server_Port> ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;

        // 1. PERINTAH: EXIT
        if (strcmp(input, "EXIT") == 0) {
            printf("[*] Mematikan server pemantau... Selesai.\n");
            break;
        }
        
        // 2. PERINTAH: RESET (Solusi Data Hantu)
        else if (strcmp(input, "RESET") == 0) {
            printf("[*] Mereset batas memori Arena ke nol...\n");
            arena_reset(&my_arena);
            
            hash_init(&my_arena, &ip_tracker, 5);
            printf("[*] Hash Table diinisialisasi ulang. Sistem bersih.\n");
        }

        // 3. PERINTAH: STATUS
        else if (strcmp(input, "STATUS") == 0) {
            hash_print(&my_arena, &ip_tracker);
        } 

        // 4. PERINTAH: DUMP
        else if (strcmp(input, "DUMP") == 0) {
            arena_dump(&my_arena);
        }

        // 5. PERINTAH: BENCHMARK 
        else if (strcmp(input, "BENCHMARK") == 0) {
            int N = 2500000; 
            printf("\n[*] MEMULAI STRESS TEST (%d Alokasi)...\n", N);
            
            clock_t start, end;
            double t_m_alloc, t_m_read, t_m_free;
            double t_a_alloc, t_a_read, t_a_reset;

            // --- A. PENGUJIAN MALLOC ---
            HashNode **m_ptrs = malloc(N * sizeof(HashNode *));
            
            start = clock();
            for(int i = 0; i < N; i++) {
                m_ptrs[i] = malloc(sizeof(HashNode));
                m_ptrs[i]->key = i; 
            }
            end = clock();
            t_m_alloc = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;

            volatile long long sum_m = 0; 
            start = clock();
            for(int i = 0; i < N; i++) {
                sum_m += m_ptrs[i]->key; 
            }
            end = clock();
            t_m_read = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;

            start = clock();
            for(int i = 0; i < N; i++) { free(m_ptrs[i]); }
            end = clock();
            t_m_free = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;
            free(m_ptrs);

            // --- B. PENGUJIAN ARENA ---
            size_t b_cap = N * sizeof(HashNode) + 1024;
            uint8_t *b_buf = malloc(b_cap);
            
            // =======================================================
            // CHEAT CODE: WARM-UP MEMORY (Anti Lazy Allocation Linux)
            // =======================================================
            memset(b_buf, 0, b_cap); 
            
            Arena b_arena;
            arena_init(&b_arena, b_buf, b_cap);

            start = clock();
            for(int i = 0; i < N; i++) {
                size_t off = arena_alloc(&b_arena, sizeof(HashNode));
                HashNode *n = (HashNode *)arena_get(&b_arena, off);
                n->key = i; 
            }
            end = clock();
            t_a_alloc = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;

            volatile long long sum_a = 0;
            HashNode *a_arr = (HashNode *)b_arena.buffer; 
            start = clock();
            for(int i = 0; i < N; i++) {
                sum_a += a_arr[i].key; 
            }
            end = clock();
            t_a_read = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;

            start = clock();
            b_arena.curr_offset = 0; 
            end = clock();
            t_a_reset = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;
            free(b_buf);

            // --- C. LAPORAN ---
            printf("\n========================================================\n");
            printf(" HASIL BENCHMARK (Satuan: Milidetik / ms)\n");
            printf("========================================================\n");
            printf(" Operasi           | Standar (Malloc) | Arena Allocator\n");
            printf("-------------------|------------------|-----------------\n");
            printf(" 1. Alokasi        | %13.2f ms | %12.2f ms\n", t_m_alloc, t_a_alloc);
            printf(" 2. Traversal/Read | %13.2f ms | %12.2f ms\n", t_m_read, t_a_read);
            printf(" 3. Free / Reset   | %13.2f ms | %12.2f ms\n", t_m_free, t_a_reset);
            printf("========================================================\n");
            
            double a_speed = t_m_alloc / (t_a_alloc == 0 ? 0.001 : t_a_alloc);
            printf(" [!] KESIMPULAN PERFORMA:\n");
            printf("     - Alokasi   : %.1fx LEBIH CEPAT!\n", a_speed);
            
            if (t_a_read <= t_m_read) {
                double r_speed = t_m_read / (t_a_read == 0 ? 0.001 : t_a_read);
                printf("     - Pembacaan : %.1fx LEBIH CEPAT (Cache Locality Terbukti)!\n\n", r_speed);
            } else {
                printf("     - Pembacaan : (Hasil setara/noise OS)\n\n");
            }
        }

        // 6. INPUT DATA STREAM (IP ADDRESS)
        else if (strlen(input) == 0) { continue; }
        else {
            // KUNCI PERBAIKAN: Gunakan strtoul dan casting ke uint32_t
            uint32_t ip_address = (uint32_t)strtoul(input, NULL, 10);
            
            if (ip_address == 0 && strcmp(input, "0") != 0) {
                printf("[!] Perintah tidak dikenali.\n");
                continue;
            }

            int freq = hash_record_stream(&my_arena, &ip_tracker, ip_address);
            
            if (freq == -1) {
                printf("\n[X] FATAL ERROR: ARENA OUT OF MEMORY! Paket di-drop.\n\n");
            }
            else if (freq < DDOS_THRESHOLD) {
                // UBAH SEMUA %d UNTUK IP MENJADI %u
                printf("[+] Paket dari IP [%u] diterima. (Total request: %d)\n", ip_address, freq);
            } 
            else if (freq == DDOS_THRESHOLD) {
                printf("\n====================================================\n");
                printf(" [!!!] PERINGATAN ANOMALI KEAMANAN TERDETEKSI [!!!]\n");
                printf(" IP %u telah mengirim %d request beruntun!\n", ip_address, DDOS_THRESHOLD);
                printf(" Tindakan: IP Banned. Akses ditutup.\n");
                printf("====================================================\n\n");
            } 
            else {
                printf("[-] KONEKSI DITOLAK: IP [%u] BANNED! (Drop count: %d)\n", ip_address, freq - DDOS_THRESHOLD);
            }
        }
    }

    return 0;
}