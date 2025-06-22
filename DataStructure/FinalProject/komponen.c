#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define ATAS 72
#define BAWAH 80
#define ENTER 13

// Struktur data
typedef struct {
    char nomorKomponen[7];
    char namaKomponen[26];
    int jumlahStok;
    double hargaSatuan;
} Komponen;

typedef struct {
    char nomorKomponen[7];
    long posisi;
} Index;

// File dan index global
const char* DATA_FILE = "komponen.dat";
const char* INDEX_FILE = "index.dat";
const char* CSV_FILE = "sample_komponen.csv";

Index indexList[1000];
int indexCount = 0;

// Fungsi bantu
void pressEnter() {
    printf("Press Enter to continue...");
    getchar(); getchar();
}

void loadIndex() {
    FILE* file = fopen(INDEX_FILE, "rb");
    if (file != NULL) {
        indexCount = 0;
        while (fread(&indexList[indexCount], sizeof(Index), 1, file) == 1) {
            indexCount++;
        }
        fclose(file);
    }
}

void saveIndex() {
    FILE* file = fopen(INDEX_FILE, "wb");
    if (file != NULL) {
        for (int i = 0; i < indexCount; i++) {
            fwrite(&indexList[i], sizeof(Index), 1, file);
        }
        fclose(file);
    }
}

void sortIndex() {
    for (int i = 0; i < indexCount - 1; i++) {
        for (int j = 0; j < indexCount - i - 1; j++) {
            if (strcmp(indexList[j].nomorKomponen, indexList[j + 1].nomorKomponen) > 0) {
                Index temp = indexList[j];
                indexList[j] = indexList[j + 1];
                indexList[j + 1] = temp;
            }
        }
    }
}

void rebuildDataFile() {
    Komponen komponenList[1000];
    int komponenCount = 0;
    FILE* inFile = fopen(DATA_FILE, "rb");
    if (inFile != NULL) {
        for (int i = 0; i < indexCount; i++) {
            fseek(inFile, indexList[i].posisi, SEEK_SET);
            fread(&komponenList[komponenCount], sizeof(Komponen), 1, inFile);
            komponenCount++;
        }
        fclose(inFile);
    }
    FILE* outFile = fopen(DATA_FILE, "wb");
    if (outFile != NULL) {
        for (int i = 0; i < komponenCount; i++) {
            long pos = ftell(outFile);
            fwrite(&komponenList[i], sizeof(Komponen), 1, outFile);
            strcpy(indexList[i].nomorKomponen, komponenList[i].nomorKomponen);
            indexList[i].posisi = pos;
        }
        fclose(outFile);
        saveIndex();
    }
}

void loadDataFromCSV() {
    printf("Loading data from CSV file...\n");
    FILE* csvFile = fopen(CSV_FILE, "r");
    if (csvFile == NULL) {
        printf("Error: File 'sample_komponen.csv' tidak ditemukan!\n");
        return;
    }
    FILE* dataFile = fopen(DATA_FILE, "wb");
    if (dataFile == NULL) {
        printf("Error creating data file!\n");
        fclose(csvFile);
        return;
    }
    char line[256];
    int count = 0;
    fgets(line, sizeof(line), csvFile);
    while (fgets(line, sizeof(line), csvFile) != NULL && strlen(line) > 1) {
        line[strcspn(line, "\n")] = 0;
        Komponen komp;
        char* token = strtok(line, ",");
        int field = 0;
        while (token != NULL && field < 4) {
            switch (field) {
                case 0: strncpy(komp.nomorKomponen, token, 6); komp.nomorKomponen[6] = '\0'; break;
                case 1: strncpy(komp.namaKomponen, token, 25); komp.namaKomponen[25] = '\0'; break;
                case 2: komp.jumlahStok = atoi(token); break;
                case 3: komp.hargaSatuan = atof(token); break;
            }
            token = strtok(NULL, ",");
            field++;
        }
        if (field >= 4) {
            long pos = ftell(dataFile);
            fwrite(&komp, sizeof(Komponen), 1, dataFile);
            strcpy(indexList[indexCount].nomorKomponen, komp.nomorKomponen);
            indexList[indexCount].posisi = pos;
            indexCount++;
            count++;
        }
    }
    fclose(csvFile);
    fclose(dataFile);
    sortIndex();
    saveIndex();
    printf("%d records loaded from CSV successfully!\n", count);
    pressEnter();
}

void tambahData() {
    Komponen komp;
    printf("Nomor Komponen (6 digit): ");
    scanf("%s", komp.nomorKomponen);
    for (int i = 0; i < indexCount; i++) {
        if (strcmp(indexList[i].nomorKomponen, komp.nomorKomponen) == 0) {
            printf("Nomor komponen sudah ada!\n");
            pressEnter();
            return;
        }
    }
    printf("Nama Komponen (max 25 char): ");
    getchar();
    fgets(komp.namaKomponen, 26, stdin);
    komp.namaKomponen[strcspn(komp.namaKomponen, "\n")] = 0;
    printf("Jumlah Stok: ");
    scanf("%d", &komp.jumlahStok);
    printf("Harga Satuan: ");
    scanf("%lf", &komp.hargaSatuan);
    FILE* file = fopen(DATA_FILE, "ab");
    if (file != NULL) {
        long pos = ftell(file);
        fwrite(&komp, sizeof(Komponen), 1, file);
        fclose(file);
        strcpy(indexList[indexCount].nomorKomponen, komp.nomorKomponen);
        indexList[indexCount].posisi = pos;
        indexCount++;
        sortIndex();
        saveIndex();
        printf("\nData berhasil ditambahkan!\n");
    } else {
        printf("Error opening file!\n");
    }
    pressEnter();
}

void ubahData() {
    char nomorCari[7];
    printf("Masukkan nomor komponen yang akan diubah: ");
    scanf("%s", nomorCari);
    long posisi = -1;
    for (int i = 0; i < indexCount; i++) {
        if (strcmp(indexList[i].nomorKomponen, nomorCari) == 0) {
            posisi = indexList[i].posisi;
            break;
        }
    }
    if (posisi == -1) {
        printf("Data tidak ditemukan!\n");
        pressEnter();
        return;
    }
    FILE* file = fopen(DATA_FILE, "r+b");
    if (file == NULL) {
        printf("Error opening file!\n");
        pressEnter();
        return;
    }
    fseek(file, posisi, SEEK_SET);
    Komponen komp;
    fread(&komp, sizeof(Komponen), 1, file);
    printf("\nData saat ini:\n");
    printf("Nomor: %s\n", komp.nomorKomponen);
    printf("Nama: %s\n", komp.namaKomponen);
    printf("Stok: %d\n", komp.jumlahStok);
    printf("Harga: %.2f\n\n", komp.hargaSatuan);
    getchar();
    printf("Nama Komponen baru (max 25 char): ");
    fgets(komp.namaKomponen, 26, stdin);
    komp.namaKomponen[strcspn(komp.namaKomponen, "\n")] = 0;
    printf("Jumlah Stok baru: ");
    scanf("%d", &komp.jumlahStok);
    printf("Harga Satuan baru: ");
    scanf("%lf", &komp.hargaSatuan);
    fseek(file, posisi, SEEK_SET);
    fwrite(&komp, sizeof(Komponen), 1, file);
    fclose(file);
    printf("\nData berhasil diubah!\n");
    pressEnter();
}

void tampilkanDataDenganIndex() {
    FILE* file = fopen(DATA_FILE, "rb");
    if (file == NULL) {
        printf("Error opening file!\n");
        pressEnter();
        return;
    }
    printf("%-8s%-8s%-26s%-8s%-12s%-15s\n", "No.", "Nomor", "Nama Komponen", "Stok", "Harga", "Total Nilai");
    printf("--------------------------------------------------------------------------------\n");
    double totalAset = 0;
    for (int i = 0; i < indexCount; i++) {
        fseek(file, indexList[i].posisi, SEEK_SET);
        Komponen komp;
        fread(&komp, sizeof(Komponen), 1, file);
        double totalNilai = komp.jumlahStok * komp.hargaSatuan;
        totalAset += totalNilai;
        printf("%-8d%-8s%-26s%-8d%-12.2f%-15.2f\n", i + 1, komp.nomorKomponen, komp.namaKomponen, komp.jumlahStok, komp.hargaSatuan, totalNilai);
    }
    fclose(file);
    printf("================================================================================\n");
    printf("Total Aset Komponen: Rp %.2f\n", totalAset);
    pressEnter();
}

void tampilkanDataTanpaIndex() {
    FILE* file = fopen(DATA_FILE, "rb");
    if (file == NULL) {
        printf("Error opening file!\n");
        pressEnter();
        return;
    }

    printf("%-8s%-26s%-8s%-12s%-15s\n", "No.", "Nama Komponen", "Stok", "Harga", "Total Nilai");
    printf("----------------------------------------------------------------------\n");

    Komponen komp;
    double totalAset = 0;
    int count = 0;

    while (fread(&komp, sizeof(Komponen), 1, file) == 1) {
        double totalNilai = komp.jumlahStok * komp.hargaSatuan;
        totalAset += totalNilai;
        count++;

        printf("%-8d%-26s%-8d%-12.2f%-15.2f\n", count, komp.namaKomponen, komp.jumlahStok, komp.hargaSatuan, totalNilai);
    }

    fclose(file);
    printf("================================================================================\n");
    printf("Total Aset Komponen: Rp %.2f\n", totalAset);
    pressEnter();
}


void hapusData() {
    char nomorCari[7];
    printf("Masukkan nomor komponen yang akan dihapus: ");
    scanf("%s", nomorCari);
    int indexPos = -1;
    for (int i = 0; i < indexCount; i++) {
        if (strcmp(indexList[i].nomorKomponen, nomorCari) == 0) {
            indexPos = i;
            break;
        }
    }
    if (indexPos == -1) {
        printf("Data tidak ditemukan!\n");
        pressEnter();
        return;
    }
    FILE* file = fopen(DATA_FILE, "rb");
    Komponen komp;
    if (file != NULL) {
        fseek(file, indexList[indexPos].posisi, SEEK_SET);
        fread(&komp, sizeof(Komponen), 1, file);
        fclose(file);
        printf("\nData yang akan dihapus:\n");
        printf("Nomor: %s\n", komp.nomorKomponen);
        printf("Nama: %s\n", komp.namaKomponen);
        printf("Stok: %d\n", komp.jumlahStok);
        printf("Harga: %.2f\n", komp.hargaSatuan);
        char konfirmasi;
        printf("\nYakin ingin menghapus? (y/n): ");
        scanf(" %c", &konfirmasi);
        if (konfirmasi == 'y' || konfirmasi == 'Y') {
            for (int i = indexPos; i < indexCount - 1; i++) {
                indexList[i] = indexList[i + 1];
            }
            indexCount--;
            saveIndex();
            rebuildDataFile();
            printf("Data berhasil dihapus!\n");
        } else {
            printf("Penghapusan dibatalkan.\n");
        }
    }
    pressEnter();
}

void exportDataToCSV() {
    FILE* file = fopen(DATA_FILE, "rb");
    if (file == NULL) {
        printf("Gagal membuka file data!\n");
        pressEnter();
        return;
    }

    FILE* csv = fopen(CSV_FILE, "w");
    if (csv == NULL) {
        printf("Gagal membuat file CSV!\n");
        fclose(file);
        pressEnter();
        return;
    }

    fprintf(csv, "Nomor,Nama,Stok,Harga\n");

    Komponen komp;
    while (fread(&komp, sizeof(Komponen), 1, file) == 1) {
        fprintf(csv, "%s,%s,%d,%.2f\n",
            komp.nomorKomponen,
            komp.namaKomponen,
            komp.jumlahStok,
            komp.hargaSatuan);
    }

    fclose(file);
    fclose(csv);
    printf("Data berhasil diekspor ke %s!\n", CSV_FILE);
    pressEnter();
}

int menu() {
    const char menu[6][100] = {
        "1. \033[4mA\033[0m Tambah Data",
        "2. \033[4mB\033[0m Ubah Data",
        "3. \033[4mC\033[0m Tampilkan Semua Data (dengan Index)",
        "4. \033[4mD\033[0m Tampilkan Semua Data (tanpa Index)",
        "5. \033[4mE\033[0m Hapus Data",
        "6. \033[4mF\033[0m Keluar"
    };

    int posisi = 0, totalMenu = 6;
    char key;

    while (1) {
        system("cls");
        printf("================================\n");
        printf("  SISTEM PENGELOLAAN KOMPONEN   \n");
        printf("================================\n");
        for (int i = 0; i < totalMenu; i++) {
            if (i == posisi)
                printf(" >> %s <<\n", menu[i]);
            else
                printf("   %s\n", menu[i]);
        }

        key = getch();
        if (key == 0 || key == -32 || key == 224) {
            key = getch();
            if (key == ATAS)
                posisi = (posisi - 1 + totalMenu) % totalMenu;
            else if (key == BAWAH)
                posisi = (posisi + 1) % totalMenu;
        } else if (key == ENTER) {
            return posisi + 1;
        } else {
            if (key >= '1' && key <= '6') {
                return key - '0';
            } else if (key >= 'A' && key <= 'F') {
                return (key - 'A') + 1;
            } else if (key >= 'a' && key <= 'f') {
                return (key - 'a') + 1;
            }
        }
    }
}

int main() {
    loadIndex();
    if (indexCount == 0) {
        loadDataFromCSV();
    }
    int pilihan;
    while (1) {
        pilihan = menu();
        if (pilihan == 1) {
            tambahData();
        } else if (pilihan == 2) {
            ubahData();
        } else if (pilihan == 3) {
            tampilkanDataDenganIndex();
        } else if (pilihan == 4) {
            tampilkanDataTanpaIndex();
        } else if (pilihan == 5) {
            hapusData();
        } else if (pilihan == 6) {
            exportDataToCSV();
            printf("Keluar dari program.\n");
            return 0;
        }
    }
}
