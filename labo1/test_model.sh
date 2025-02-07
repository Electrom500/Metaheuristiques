!/bin/bash

for file in wt*.txt; do
    for methode in 0 1 2; do
        for nb_voisins in 1 2 5 10 20; do
            echo "dist/release/descente $file $nb_voisins 10000 $methode 1000 resultats_${file%.*}.csv"
            dist/release/descente $file $nb_voisins 10000 $methode 1000 resultats_${file%.*}.csv
        done
    done
done