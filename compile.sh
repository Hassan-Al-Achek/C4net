#!/bin/bash

for i in $(seq 1 4)
do
        gcc "etape$i.c" -o "etape$i"
done

echo "[+] Compilation Done"
 