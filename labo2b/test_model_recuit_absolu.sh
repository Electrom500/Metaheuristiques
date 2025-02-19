#!/bin/bash

for file in wt*.txt; do
    for i in $(seq 0 1000); do
        # Meilleur en écart absolu quadratique
        dist/release/recuit $file 2 375 0.75 15 100000 resultats_recuit_meilleur_absolu.csv
    done
done