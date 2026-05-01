# Dokumentasi Proyek: Arena Allocator - Struktur Data Dasar (Minggu 2)

## 1. Pendahuluan
Pada minggu kedua ini, kita mengembangkan penggunaan **Arena Allocator** untuk menampung struktur data yang lebih kompleks, yaitu **Array** dan **Linked List**. 

Konsep terpenting yang diperkenalkan pada minggu ini adalah **Linked List Berbasis Offset**. Berbeda dengan Linked List tradisional yang menggunakan pointer memory asli (seperti `Node *next`), implementasi kita menggunakan indeks offset (`size_t next_offset`). Hal ini membuat data di dalam arena bersifat *relocatable* (bisa dipindahkan secara utuh) tanpa merusak referensi antar node.

---

## 2. Struktur Data Baru

### 2.1. Linked List Node (`linkedlist.h`)
```c
// Konstanta penanda akhir list (sebagai pengganti NULL)
#define ARENA_NULL (size_t)-1 

typedef struct {
    int data;
    size_t next_offset; // Bukan pointer Node*, melainkan jarak dari awal arena
} Node;
```
- **`data`**: Menyimpan nilai integer.
- **`next_offset`**: Menyimpan posisi node selanjutnya dalam bentuk angka offset (jarak byte dari titik `0` arena). Jika node adalah elemen terakhir, nilainya diset ke `ARENA_NULL`.

---

## 3. Analisis Fungsi Linked List (`linkedlist.c`)

### `list_create_node`
Membuat node baru di dalam arena.
1. Meminta alokasi memori sebesar `sizeof(Node)` dari arena (`arena_alloc`).
2. Menggunakan `arena_get` untuk mendapatkan pointer sementara guna mengisi `data` dan `next_offset`.
3. Mengembalikan nilai offset dari node tersebut (bukan pointernya).

### `list_append`
Menambahkan node di akhir list.
- **Logika**:
  - Jika list masih kosong (`*head_offset == ARENA_NULL`), node baru langsung menjadi *head*.
  - Jika sudah ada isinya, fungsi akan menelusuri (*traverse*) list dari head, melompat dari satu offset ke offset berikutnya hingga menemukan node dengan `next_offset == ARENA_NULL`.
  - Node terakhir kemudian disambungkan ke node baru dengan memperbarui nilai `next_offset`-nya.

### `list_traverse`
Menelusuri dan mencetak isi list.
- Fungsi ini menelusuri list menggunakan prinsip yang sama: mengubah offset menjadi pointer menggunakan `arena_get` untuk membaca data, lalu bergerak ke `next_offset`, berulang hingga mencapai `ARENA_NULL`.

---

## 4. Alokasi Array Kontigu (`main.c`)
Arena Allocator juga sangat cocok untuk mengalokasikan array (memori yang berurutan secara fisik).
```c
size_t array_offset = arena_alloc(&my_arena, 5 * sizeof(int));
int *arr = (int *)arena_get(&my_arena, array_offset);
```
- Hanya dengan satu panggilan `arena_alloc`, kita mendapatkan blok memori besar yang cukup untuk 5 integer.
- Pengaksesan elemen array dilakukan seperti biasa menggunakan pointer aslinya (`arr[i]`).

---

## 5. Visualisasi Memori: `arena_dump`
Sebuah fungsi utilitas baru `arena_dump` ditambahkan di `arena.c` untuk melihat representasi fisik dari buffer memori.

Fungsi ini mencetak isi buffer arena dalam format *grid hexadecimal* (16 byte per baris). 
- Bagian yang sudah dialokasikan (dibawah `curr_offset`) akan menampilkan nilai heksadesimal aslinya.
- Bagian yang masih kosong (di atas `curr_offset`) akan ditampilkan sebagai `--`.

Fungsi ini sangat berguna untuk proses *debugging* dan mengamati bahwa data Array maupun node-node Linked List dialokasikan secara berurutan dan rapat (tanpa fragmentasi eksternal) sesuai dengan prinsip *Bump Allocation*.

---

## 6. Alur Eksekusi (`main.c`)
1. **Inisialisasi**: Arena disiapkan dengan kapasitas 512 bytes.
2. **Pengujian Array**: Mengalokasikan 5 integer, mengisi nilainya dengan kelipatan 10, dan mencetak alamat masing-masing elemen untuk membuktikan bahwa mereka bersebelahan secara fisik di memori.
3. **Pengujian Linked List**: Menginisialisasi list kosong, menambahkan 3 node (1999, 2026, 9999), dan mencetak isinya. Walaupun terlihat seperti Linked List biasa, hubungannya direkatkan oleh index offset.
4. **Dump Memori**: Memanggil `arena_dump` untuk melihat "jejak" dari array dan linked list yang baru saja dibuat di dalam memori fisik arena.

---
## 7. Kesimpulan
Pada implementasi ini, kita berhasil memisahkan konsep manajemen memori (Arena) dengan struktur data (Linked List). Dengan menggunakan metode referensi offset, struktur data di dalam arena menjadi lebih aman, stabil, dan berpotensi untuk di-*serialize* (disimpan ke disk) dengan mudah di masa mendatang.
