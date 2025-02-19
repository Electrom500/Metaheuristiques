#!/bin/bash

for file in wt*.txt; do
    for i in $(seq 0 1000); do
        # Meilleur en écart absolu quadratique
        dist/release/tabou $file 45 49 100000 resultats_tabou_meilleur_absolu.csv
    done
done