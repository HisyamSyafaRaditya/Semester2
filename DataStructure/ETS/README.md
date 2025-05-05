# ```LAPORAN ETS STRUKTUR DATA```
---
## 1. ```Soal 1```
Sebuah robot bergerak di dalam ruangan 2 dimensi berdasarkan perintah string yang diberikan. 

Robot dapat menerima perintah sebagai berikut: 

- 'U' → gerak ke atas (y+1) 
- 'D' → gerak ke bawah (y−1) 
- 'L' → gerak ke kiri (x−1) 
- 'R' → gerak ke kanan (x+1) 
- 'B' → membatalkan satu gerakan terakhir yang belum pernah dibatalkan (undo), menggunakan stack 

Tulislah program dalam bahasa C yang membaca string perintah dan menghitung posisi akhir robot. Setelah semua perintah dijalankan, dengan mempermbangkan pembatalan perintah melalui 
mekanisme stack. 

**Ketentuan:**

Program harus mengimplementasikan struktur data stack secara manual ( dak menggunakan library 
eksternal). Perintah 'B' bisa muncul beberapa kali dan se ap kali harus membatalkan satu perintah gerakan 
sebelumnya (U, D, L, R) — bukan 'B' lain. Jika 'B' muncul lebih banyak dari jumlah perintah gerakan sebelumnya, maka sisa 'B' diabaikan. 

Posisi awal robot adalah (0, 0).

``Penjelasan Program``

``1. Header``
```
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
```
- **stdio.h**: Untuk fungsi input-output.
- **string.h**: Untuk operasi string (strlen).
- **stdlib.h**: Untuk fungsi malloc dan free.
- **stdbool.h**: Untuk menggunakan tipe data boolean (true/false).

``2. Pergerakan Robot``
```
void up(int *x, int *y){
    *x = *x;
    *y = *y + 1;
}

void down(int *x, int *y){
    *x = *x;
    *y = *y - 1;
}

void left(int *x, int *y){
    *x = *x - 1;
    *y = *y;
}

void right(int *x, int *y){
    *x = *x + 1;
    *y = *y;
}
```
- **up**: Menambah y (bergerak ke atas).
- **down**: Mengurangi y (bergerak ke bawah).
- **left**: Mengurangi x (bergerak ke kiri).
- **right**: Menambah x (bergerak ke kanan).

``3. Struktur Data Stack``
```
typedef struct tNode{
    char c;
    struct tNode *next;
    struct tNode *prev;
} Node;
```
- Akan menyimpan satu karakter sebagai arah gerak robot dan pointer ke karakter (perintah) sebelum dan sesudahnya.

``4. Struktur Stack``
```
typedef struct{
    Node *head;
    Node *tail;
} Stack;
```
Menyimpan alamat stack paling depan dan belakang.

``5. Fungsi Dasar Untuk Tipe Data Stack``
```
int empty(Stack *s){
    return (s->head == NULL && s->tail == NULL);
}

void push(Stack *s, char x){
    Node *new = (Node *)malloc(sizeof(Node));
    new->c = x;
    new->next = NULL;
    new->prev = NULL;
    if(empty(s)){
        s->head = new;
        s->tail = new;
    } else {
        new->prev = s->tail;
        s->tail->next = new;
        s->tail = new;
    }
}

void pop(Stack *s){
    if(!empty(s)){
        Node *temp = s->tail;
        if(s->head == s->tail){
            s->head = NULL;
            s->tail = NULL;
        } else {
            s->tail = s->tail->prev;
            s->tail->next = NULL;
        }
        free(temp);
    }
}
```
- Fungsi untuk mengecek apakah stack masih kosong
- fungsi untuk memasukkan data ke dalam stack
- fungsi untuk menghapus karakter paling depan pada stack

``6. Fungsi main``
```
    Stack s;
    s.head = NULL;
    s.tail = NULL;

    char cmd[512];
    scanf("%s", cmd);

    for(int i = 0; i < strlen(cmd); i++){
        if(cmd[i] == 'B'){
            pop(&s);
        } else{
            push(&s, cmd[i]);
        }
    }
```
- Stack diinisialisasi kosong.
- Membaca input string berisi perintah (maksimal 511 karakter + null terminator).

``7. Memproses string yang diinput``
```
    for(int i = 0; i < strlen(cmd); i++){
        if(cmd[i] == 'B'){
            pop(&s);
        } else{
            push(&s, cmd[i]);
        }
    }
```
- Jika perintah 'B', hapus perintah terakhir (simulasi backspace).
- Jika bukan 'B', tambahkan karakter ke stack.

``8. Menghitung koordinat robot di setiap gerakan``
```
    int x = 0;
    int y = 0;

    Node *current = s.head;
    while(current != NULL){
        if(current->c == 'U'){
            up(&x, &y);
        } else if(current->c == 'D'){
            down(&x, &y);
        } else if(current->c == 'L'){
            left(&x, &y);
        } else if(current->c == 'R'){
            right(&x, &y);
        }
        current = current->next;
    }
```
- Looping setiap karakter pada stack dan memanggil fungsi untuk gerakan robot `(langkah ke-2)`.

``9. Output akhir``
```
printf("Posisi akhir robot : (%d, %d)\n", x, y);
```
### `Contoh Input dan Output`
```
Input : UURDBBL
Output : Posisi akhir robot : (-1, 2)
```

---
## 2. ```Soal 2```
**Sebuah pusat panggilan darurat menerima laporan dari masyarakat. Setiap laporan memiliki:**
- id laporan (integer unik), 
- nama pelapor (maks. 30 karakter), 
- kode prioritas (integer antara 1 sampai 5; 1 paling kri s, 5 paling ringan), 
- timestamp kedatangan (urutan waktu kedatangan laporan, bertambah setiap laporan masuk). 

**Pusat panggilan akan memproses laporan berdasarkan aturan sebagai berikut:** 
- Laporan dengan prioritas lebih nggi (angka lebih kecil) diproses lebih dulu. 
- Jika ada beberapa laporan dengan prioritas yang sama, maka laporan yang datang lebih dulu 
( mestamp lebih kecil) diproses lebih dahulu. 
- Gunakan linked list (tidak boleh array, queue STL, atau library list lainnya).

**Buat menu interaktif berikut:** 
1. Tambah laporan baru 
2. Proses laporan (hapus dari list dan tampilkan datanya) 
3. Tampilkan semua laporan dalam antrean 
4. Keluar dari program 

``Penjelasan Program :``

``1. Struktur Node``
```
typedef struct Node{
    int id;
    char nama[50];
    int priority;
    char timestamp[50];
    struct Node *next;
    struct Node *prev;
} tNode;
```
- **id**: ID unik laporan (otomatis bertambah).
- **nama**: Nama pelapor.
- **priority**: Nilai prioritas (semakin kecil, semakin prioritas).
- **timestamp**: Waktu laporan dimasukkan.
- **next, prev**: Pointer ke node selanjutnya dan sebelumnya.

``2. Struct Queue``
```
typedef struct{
    tNode *head;
    tNode *tail;
    int size;
} Queue;
```
- **head**: Node dengan prioritas tertinggi.
- **tail**: Node terakhir.
- **size**: Jumlah laporan dalam antrean.

``3. Fungsi Cek apakah Queue Kosong``
```
bool empty(Queue *arr){
    return (arr->head == NULL && arr->tail == NULL);
}
```

``4. Fungsi Untuk Print dan Menghapus Queue Paling Depan``
```
void pop(Queue *arr){
    if(!empty(arr)){
        tNode *temp = arr->head;

        printf("ID Laporan : %d\n", temp->id);
        printf("Nama Pelapor : %s\n", temp->nama);
        printf("Kode Prioritas : %d\n", temp->priority);
        printf("Timestamp : %s\n", temp->timestamp);

        arr->head = arr->head->next;
        if(arr->head == NULL){
            arr->tail = NULL;
        } else{
            arr->head->prev = NULL;
        }
        free(temp);
        arr->size--;
    }
}
```
- Print antrean laporan pada urutan paling depan. 
- Menggeser head (node paling depan) ke node urutan ke-2.
- Menghapus variabel temp yang menyimpan node paling depan(sebelum digeser) menggunakan **free(temp)**.

``5. Inisialisasi Queue``
```
Queue arr;
arr.head = NULL;
arr.tail = NULL;
arr.size = 0;
```
- Inisialisasi queue kosong terlebih dahulu.

``6. Menampilkan menu interaktif``
```
Menu :
1. Tambah laporan baru
2. Proses laporan
3. Tampilkan semua laporan dalam antrean
4. Keluar dari program
```

``7. Memasukkan Laporan Baru``
```
    tNode *new = (tNode *)malloc(sizeof(tNode));
    new->next = NULL;
    new->prev = NULL;

    new->id = currentId;
    currentId++;
    printf("Nama : ");
    scanf("%s", new->nama);
    printf("Tingkat prioritas : ");
    scanf("%d", &new->priority);
```
- Mengalokasikan memori untuk node baru mengunakan `malloc`.
- Menerima input dari user (nama dan tingkat prioritas).

``8. Mencatat Timestamp``
```
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(new->timestamp, sizeof(new->timestamp), "%d-%m-%y %H:%M:%S", t);
```
- Menggunakan header `time.h` untuk mencatat timestamp dengan format yang bebas ditentukan

``9. Memasukkan Laporan ke Queue``
```
    if(empty(&arr)){
        arr.head = new;
        arr.tail = new;
    } else{
        tNode *current = arr.head;
        tNode *prev = NULL;
        while(current != NULL && current->priority <= new->priority){
            prev = current;
            current = current->next;
        }
        if(prev == NULL){
            new->next = arr.head;
            arr.head->prev = new;
            arr.head = new;
        } else if(current == NULL){
            arr.tail->next = new;
            new->prev = arr.tail;
            arr.tail = new;
        } else{
            new->prev = prev;
            new->next = current;
            current->prev = new;
            prev->next = new;   
        }
    }
```
- Jika antrean kosong: langsung menjadi head dan tail.
- Jika prioritas lebih tinggi dari semua: disisipkan di depan (head).
- Jika prioritas paling rendah: ditambahkan di akhir (tail).
- Jika di tengah: disisipkan di antara dua node.

``10. Fungsi Untuk Memproses Laporan``
```
} else if(n == 2){
    pop(&arr);
}
```
- memanggil fungsi `pop()` untuk print data laporan paling depan dan menghapusnya dari antrean.

``11. Print Semua Laporan yang Masuk``
```
} else if(n == 3){
    tNode *current = arr.head;
    while(!empty(&arr)){
        pop(&arr);
        printf("\n");
    }
}
```
- Memanggil fungsi pop secara berulang sampai antrean kosong.

### `Contoh Input dan Output`
```
Menu :
1. Tambah laporan baru
2. Proses laporan
3. Tampilkan semua laporan dalam antrean
4. Keluar dari program
Masukkan pilihan menu : 1
Nama : h1
Tingkat prioritas : 1
Masukkan pilihan menu : 1
Nama : h2
Tingkat prioritas : 2
Masukkan pilihan menu : 1
Nama : h3
Tingkat prioritas : 3
Masukkan pilihan menu : 1
Nama : h4
Tingkat prioritas : 1
Masukkan pilihan menu : 2
ID Laporan : 1
Nama Pelapor : h1
Kode Prioritas : 1
Timestamp : 05-05-25 21:20:22
Masukkan pilihan menu : 3
ID Laporan : 4
Nama Pelapor : h4
Kode Prioritas : 1
Timestamp : 05-05-25 21:20:36

ID Laporan : 2
Nama Pelapor : h2
Kode Prioritas : 2
Timestamp : 05-05-25 21:20:25

ID Laporan : 3
Nama Pelapor : h3
Kode Prioritas : 3
Timestamp : 05-05-25 21:20:30

Masukkan pilihan menu : 4
Program dihentikan!
```

---
## 3. ```Soal 3```
**Sebuah keluarga besar menyimpan silsilah pewarisan harta dalam bentuk binary tree. Setiap node pada pohon merepresentasikan satu anggota keluarga dengan:**
- id anggota (unik, integer), 
- nama (maks. 30 karakter), 
- nilai warisan (dalam jutaan rupiah, integer ≥ 0) 

**Struktur keluarga mengiku aturan:**
- Setiap anggota keluarga maksimal memiliki dua ahli waris (anak kiri dan anak kanan). 
- Setiap kali seseorang meninggal, warisannya dibagi secara rekursif ke anak-anaknya: 
- Anak kiri mendapat 60%, anak kanan mendapat 40% dari nilai warisan orang tuanya. 
- Warisan ini ditambahkan ke nilai yang sudah dimiliki anak tersebut. 

**Tugas Anda: 
Buat program untuk:** 
1. Menambahkan anggota baru ke dalam pohon keluarga. 
2. Menampilkan struktur pohon keluarga secara inorder. 
3. Mensimulasikan kema an seorang anggota (dengan ID tertentu), dan membagi warisannya ke anak-anaknya secara rekursif mengikuti aturan di atas. 

Implementasi harus menggunakan binary tree manual ( dak boleh pakai array/tree library). 
Jika anggota yang meninggal tidak memiliki anak, warisan dianggap hangus.

`Penjelasan Program`

``1. Struktur Data``
```
typedef struct tNode{
    int key;
    float warisan;
    bool isAlive;
    struct tNode *left, *right;
} Node;
```
- **key** : ID unik anggota keluarga.
- **warisan** : Jumlah harta yang dimiliki.
- **isAlive** : Status hidup (true jika masih hidup).
- **left dan right** : Anak kiri dan kanan (BST).

``2. Struct Tree``
```
typedef struct{
    Node *root;
    int size;
} Tree;
```
- **root**: Akar pohon keluarga.
- **size**: Ukuran pohon.

``3. Fungsi-Fungsi Dasar Untuk Struktur Data Tree``
```
bool empty(Tree *t){
    return (t->root == NULL);
}

void push(Tree *t, int id, float m){
    Node *new = (Node *)malloc(sizeof(Node));
    new->key = id;
    new->warisan = m;
    new->isAlive = true;
    new->left = NULL;
    new->right = NULL;

    if(empty(t)){
        t->root = new;
    } else{
        Node *current = t->root;
        while(current != NULL){
            if(current->key > id && current->left != NULL){
                current = current->left;
            } else if(current->key < id && current->right != NULL){
                current = current->right;
            } else{
                break;
            }
        }

        if(current->key > id){
            current->left = new;
        } else if(current->key < id){
            current->right = new;
        }
    }
}

void printInorder(Node *node){
    if(node == NULL){
        return;
    }

    printInorder(node->left);
    if(node->isAlive){
        printf("ID anggota keluarga : %d\nHarta dimiliki : %.2f\n\n", node->key, node->warisan);
    }
    printInorder(node->right);
}

bool find(Node *node, int target){
    bool found = false;
    while(node != NULL){
        if(node->key < target){
            node = node->right;
        } else if(node->key > target){
            node = node->left;
        } else{
            found = true;
            break;
        }
    }
    return found;
}
```
- `empty()` : fungsi untuk mengecek apakah tree masih kosong.
- `push()` : fungsi untuk memasukkan data baru ke tree sesuai hierarki BST.
- `printInorder()` : Fungsi untuk print data dalam tree sesuai urutan inorder.
- `find()` : fungsi untuk mencari apakah suatu data (key) ada di dalam tree.

``4. Fungsi Pembagian Harta yang Diwariskan``
```
void distribusi(Node *node){
    if(node->left == NULL && node->right == NULL){
        return;
    }

    float toLeft = 0.6f * node->warisan;
    float toRight = 0.4f * node->warisan;

    bool leftAlive = node->left != NULL && node->left->isAlive;
    bool rightAlive = node->right != NULL && node->right->isAlive;

    if(rightAlive && leftAlive){
        node->left->warisan += toLeft;
        node->right->warisan += toRight;
    } else if(leftAlive){
        node->left->warisan += node->warisan;
    } else if(rightAlive){
        node->right->warisan += node->warisan;
    }
}
```
**Jika memiliki dua anak:**
- Anak kiri mendapat 60%.
- Anak kanan mendapat 40%.

**Jika hanya satu anak:**
- Anak tersebut mendapat 100% warisan.

**Jika tidak memiliki anak:**
- Warisan hilang (tidak diwariskan).

``5. Fungsi Utama Wariskan``
```
void wariskan(Tree *t, int id){
    Node *current = t->root;
    while(current->key != id){
        if(current->key < id){
            current = current->right;
        } else if(current->key > id){
            current = current->left;
        }
    }
    distribusi(current);
    current->isAlive = false;
    current->warisan = 0;
}
```
- Menandai seseorang sebagai meninggal (isAlive = false).
- Panggil distribusi() untuk membagikan hartanya.
- Set harta pribadi menjadi 0.

``Menu Utama``
```
Pilihan Menu:
1. Tambahkan anggota baru
2. Tampilkan struktur pohon keluarga secara inorder
3. Simulasi kematian seseorang
```
**Proses:**
- Menu 1: Input ID dan jumlah harta, lalu tambahkan ke pohon.
- Menu 2: Cetak anggota yang masih hidup (inorder).
- Menu 3:

    - Input ID yang meninggal.
    - Jika ditemukan, distribusi warisan dilakukan dan orang tersebut dianggap meninggal.    
    - Jika tidak ditemukan, tampilkan pesan kesalahan.

### `Contoh Input dan Output`
```
Pilihan Menu :
1. Tambahkan anngota baru
2. Tampilkan struktur pohon keluarga secara inorder
3. Simulasi kematian seseorang
Masukkan pilihan menu : 1
Masukkan id anggota keluarga baru : 2
Masukkan jumlah harta yang dimiliki : 100000
Masukkan pilihan menu : 1
Masukkan id anggota keluarga baru : 1
Masukkan jumlah harta yang dimiliki : 100000
Masukkan pilihan menu : 1
Masukkan id anggota keluarga baru : 3
Masukkan jumlah harta yang dimiliki : 100000
Masukkan pilihan menu : 3
Masukkan id anggota keluarga yang meninggal : 2
Masukkan pilihan menu : 2
ID anggota keluarga : 1
Harta dimiliki : 160000.00

ID anggota keluarga : 3
Harta dimiliki : 140000.00
```
- **Keterangan** : Harta dari (2) dipindahkan ke anak kiri (1) sebanyak 60% dan sisanya ke anak kanan(3).
