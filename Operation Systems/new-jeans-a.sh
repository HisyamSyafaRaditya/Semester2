#!/bin/bash

#unzip file
unzip newjeans_analysis.zip

# Cek apakah ada karakter tidak terlihat
cat -A DataStreamer.csv | head -5

# Hapus karakter ^M jika ada
sed -i 's/\r$//' DataStreamer.csv

# Menampilkan username yang sesuai kriteria
echo "Username yang sesuai kriteria :"

awk -F, '
    $2 ~ /2/ && $2 !~ /_/ { print $2 }
' DataStreamer.csv | sort

# Hitung jumlah username yang sesuai
count=$(awk -F, '$2 ~ /2/ && $2 !~ /_/ { count++ } END { print count }' DataStreamer.csv)

echo "Total username yang sesuai : $count"
