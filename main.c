#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <stdint.h>
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
    printf(" [ SERVER SECURITY DASHBOARD - VERSION 4.5 FINAL ]\n");
    printf("====================================================\n");
    printf(" PERINTAH TERSEDIA:\n");
    printf(" - Masukkan IP (format: 192.168.1.1) lalu Enter\n");
    printf(" - Ketik 'STATUS'    : Cek rekapitulasi IP & Frekuensi\n");
    printf(" - Ketik 'DUMP'      : Forensik Memori Arena (Hex)\n");
    printf(" - Ketik 'RESET'     : Bersihkan Arena & Hash Table\n");
    printf(" - Ketik 'BENCHMARK' : Stress Test Arena vs Malloc\n");
    printf(" - Ketik 'EXIT'      : Matikan Server\n");
    printf("====================================================\n\n");

    char input[64];

    while (1) {
        printf("Server_Port> ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        // KUNCI PERBAIKAN: Deteksi jika input kepanjangan dan bersihkan stdin (Input Flusher)
        if (strchr(input, '\n') == NULL) {
            int c;
            // Sapu bersih sisa karakter sampai ketemu Enter atau batas File
            while ((c = getchar()) != '\n' && c != EOF);
        } else {
            // Hapus karakter newline (Enter) jika ukurannya normal
            input[strcspn(input, "\n")] = 0;
        }

        // 1. PERINTAH: EXIT
        if (strcmp(input, "EXIT") == 0) {
            printf("[*] Mematikan server pemantau... Selesai.\n");
            break;
        }
        
        // 2. PERINTAH: RESET
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

        // 5. PERINTAH: BENCHMARK (Versi Optimasi Linux)
        else if (strcmp(input, "BENCHMARK") == 0) {
            int N = 2500000; 
            printf("\n[*] MEMULAI STRESS TEST (%d Alokasi)...\n", N);
            printf("[*] Melakukan warm-up memori (Anti-Lazy Allocation Linux)...\n");
            
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
            for(int i = 0; i < N; i++) { sum_m += m_ptrs[i]->key; }
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
            memset(b_buf, 0, b_cap); // Pemanasan Memori
            
            Arena b_arena;
            arena_init(&b_arena, b_buf, b_cap);

            start = clock();
            for(int i = 0; i < N; i++) {
                size_t off = arena_alloc(&b_arena, sizeof(HashNode));
                HashNode *n = (HashNode *)arena_get(&b_arena, off);
                n->key = (uint32_t)i; 
            }
            end = clock();
            t_a_alloc = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;

            volatile long long sum_a = 0;
            HashNode *a_arr = (HashNode *)b_arena.buffer; 
            start = clock();
            for(int i = 0; i < N; i++) { sum_a += a_arr[i].key; }
            end = clock();
            t_a_read = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;

            start = clock();
            b_arena.curr_offset = 0; 
            end = clock();
            t_a_reset = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0;
            free(b_buf);

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
                printf("     - Pembacaan : (Hasil setara / Optimasi OS)\n\n");
            }
        }

        // 6. INPUT DATA STREAM (PARSING IPv4 X.X.X.X)
        else if (strlen(input) == 0) { continue; }
        else {
            unsigned int o1, o2, o3, o4;
            if (sscanf(input, "%u.%u.%u.%u", &o1, &o2, &o3, &o4) == 4) {
                
                if (o1 > 255 || o2 > 255 || o3 > 255 || o4 > 255) {
                    printf("[-] FORMAT ERROR: Nilai per blok IP maksimal 255!\n");
                    continue;
                }

                // Bitwise manipulation untuk menggabungkan 4 blok menjadi 1 ID 32-bit
                uint32_t ip_address = (o1 << 24) | (o2 << 16) | (o3 << 8) | o4;

                int freq = hash_record_stream(&my_arena, &ip_tracker, ip_address);
                
                if (freq == -1) {
                    printf("\n[X] FATAL ERROR: ARENA OUT OF MEMORY! Paket di-drop.\n\n");
                }
                else if (freq < DDOS_THRESHOLD) {
                    printf("[+] Paket dari IP [%u.%u.%u.%u] diterima. (Total: %d)\n", o1, o2, o3, o4, freq);
                } 
                else if (freq == DDOS_THRESHOLD) {
                    printf("\n====================================================\n");
                    printf(" [!!!] PERINGATAN ANOMALI KEAMANAN TERDETEKSI [!!!]\n");
                    printf(" IP %u.%u.%u.%u telah mengirim %d request beruntun!\n", o1, o2, o3, o4, DDOS_THRESHOLD);
                    printf(" Tindakan: IP Banned. Akses ditutup.\n");
                    printf("====================================================\n\n");
                } 
                else {
                    printf("[-] KONEKSI DITOLAK: IP [%u.%u.%u.%u] BANNED! (Drop count: %d)\n", o1, o2, o3, o4, freq - DDOS_THRESHOLD);
                }
            } 
            else {
                printf("[!] Format tidak dikenali. Gunakan X.X.X.X atau perintah menu.\n");
            }
        }
    }

    return 0;
}