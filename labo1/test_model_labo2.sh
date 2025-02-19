!/bin/bash

for file in wt*.txt; do
    echo "dist/release/descente $file 5 100000 0 1000 resultats_descente_labo2.csv"
    dist/release/descente $file 5 100000 0 1000 "resultats_descente_labo2.csv"
done