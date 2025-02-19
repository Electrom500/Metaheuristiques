#!/bin/bash

for file in wt*.txt; do
    for i in $(seq 0 1000); do
        dist/release/recuit $file 5 400 0.80 20 100000 resultats_recuit_meilleur_relatif.csv
    done
done