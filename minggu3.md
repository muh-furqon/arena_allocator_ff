# Dokumentasi Proyek: Arena Allocator - Hash Table (Minggu 3)

## 1. Pendahuluan
Pada minggu ketiga ini, kita melakukan *upgrade* struktur data dari yang sebelumnya berupa Linked List sederhana (pada Minggu 2) menjadi **Hash Table** (*Separate Chaining*). 

Penggunaan Hash Table memungkinkan pencarian dan penyisipan data yang jauh lebih cepat (rata-rata $O(1)$) dibandingkan dengan Linked List ($O(N)$). Hash Table ini diimplementasikan secara terintegrasi dengan sistem **Arena Allocator** menggunakan metode **offset**, memastikan bahwa alokasi memori tetap deterministik, bebas *memory leak*, dan aman dari fragmentasi.

Studi kasus yang digunakan pada simulasi `main.c` minggu ini adalah **Real-Time IP Address Tracker (DDoS Detection)**, yang membuktikan bagaimana sistem dapat mencatat frekuensi kemunculan IP secara *real-time* di lingkungan memori yang sangat terbatas.

---

## 2. Perbandingan Kemajuan: Minggu 2 vs Minggu 3

| Aspek | Minggu 2 (Linked List) | Minggu 3 (Hash Table) |
| :--- | :--- | :--- |
| **Kompleksitas Pencarian** | $O(N)$ - Harus mengecek elemen satu per satu dari awal. | **$O(1)$** - Langsung melompat ke *bucket* (ember) yang tepat berdasarkan nilai *hash*. |
| **Struktur Internal** | Satu rantai linier yang terus memanjang ke belakang. | Array of Offsets (*Buckets*) di mana setiap *bucket* memegang *head* dari rantai (Linked List) masing-masing. |
| **Kasus Penggunaan** | Menyimpan daftar data yang berurutan tanpa butuh pencarian spesifik. | Menghitung frekuensi (*counting*), deteksi duplikasi data, dan pencatatan aliran data (*data stream*). |
| **Penggunaan Linked List** | Sebagai struktur data utama yang berdiri sendiri. | Menjadi sistem pendukung (*Separate Chaining*) khusus untuk menangani tabrakan data (*collision*) di *bucket* yang sama. |

---

## 3. Struktur Data Hash Table (`hashtable.h`)

### Node Data (`HashNode`)
```c
typedef struct {
    int key;            // ID unik (contoh: IP Address)
    int frequency;      // Jumlah kemunculan data
    size_t next_offset; // Offset untuk menyambung node jika terjadi tabrakan (collision)
} HashNode;
```
Berbeda dengan Node pada minggu 2, `HashNode` menyimpan *key* unik dan sistem perhitungan frekuensinya sendiri.

### Pengelola Ember (`HashTable`)
```c
typedef struct {
    size_t buckets_offset; // Offset menuju array yang menyimpan referensi *head* tiap rantai
    size_t num_buckets;    // Jumlah total *bucket* (ember) yang tersedia
} HashTable;
```

---

## 4. Analisis Mekanisme Internal (`hashtable.c`)

### Fungsi Hash
Fungsi hash yang digunakan sangat sederhana, yaitu menggunakan operator Modulo (`% num_buckets`). Fungsi ini mengubah nilai kunci besar (seperti IP) menjadi indeks array (ember) yang valid di memori.

### `hash_init` (Inisialisasi)
Mengalokasikan memori berupa **Array of `size_t`** di dalam arena sebesar jumlah ember yang diminta. Array ini kemudian dikosongkan (diisi dengan `ARENA_NULL`). Array ini bertindak sebagai tempat pendaftaran kepala (*head*) dari semua rantai Linked List.

### `hash_record_stream` (Pencatatan Data O(1))
Fungsi ini melakukan penyisipan atau pembaruan dengan mekanisme:
1. **Fase 1 (Pencarian)**: Mengubah `key` menjadi `index` ember. Membaca *head* di ember tersebut dan melakukan *traversal* (jika ada isinya). Jika `key` ditemukan sama, maka hanya `frequency` yang dinaikkan.
2. **Fase 2 (Pembuatan Node)**: Jika belum ada, fungsi akan meminta alokasi `HashNode` baru ke Arena Allocator. Node baru ini lalu disisipkan **di bagian depan** (*Insert at Head*) dari *bucket* tersebut, menggantikan *head* yang lama.

### `hash_print` (Rekapitulasi)
Menelusuri setiap *bucket*, dan jika ada isinya, fungsi ini akan mengikuti offset Linked List (`next_offset`) untuk mencetak semua `key` dan `frequency` dari tiap node.

---

## 5. Simulasi Studi Kasus: Deteksi DDoS (`main.c`)
Pada program `main.c`, kita menyimulasikan sistem yang memiliki keterbatasan memori ekstrem (Arena hanya berkapasitas **256 bytes**) dan Hash Table hanya dibekali **5 ember (buckets)**.

1. **Aliran Data**: Array berisi simulasi ribuan panggilan IP (*requests*) masuk bertubi-tubi. IP normal hanya masuk beberapa kali, namun ada IP penyerang (`ip_attacker` dan `ip_attacker2`) yang muncul berulang-ulang.
2. **Collision (*Tabrakan*)**: Karena hanya ada 5 *buckets* tetapi ada beberapa jenis IP yang berbeda (bahkan nilainya puluhan juta), tabrakan (*collision*) indeks tidak bisa dihindari.
3. **Penyelesaian Collision**: Hash Table secara otomatis membuktikan bahwa ketika 2 IP masuk ke *bucket* yang sama, mereka akan disusun menjadi rantai Linked List (*Separate Chaining*), di mana masing-masing IP tetap memiliki perhitungan frekuensinya secara akurat secara individu.
4. **Arena Dump**: Membuktikan secara fisik di RAM bagaimana alokasi *Array of Buckets* dan *Node-node Data* saling bertumpuk secara efisien berdampingan di batas memori yang diizinkan (256 byte).

---

## 6. Kesimpulan
Implementasi Hash Table menggunakan Arena Allocator menunjukkan integrasi tingkat tinggi antara struktur data kompleks dengan manajemen memori manual tingkat rendah. Penggunaan referensi Offset (`size_t`) dibandingkan Pointer (*Memory Address* murni) sukses membangun Hash Table yang *portable*, stabil, serta lebih mudah di-debug melalui fitur *Memory Dump*.
