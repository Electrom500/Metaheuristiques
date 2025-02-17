import numpy as np
import pandas as pd

instances = [
    "wt100-56.txt",
    "wt100-61.txt",
    "wt100-66.txt",
    "wt100-81.txt",
    "wt100-86.txt",
    "wt100-94.txt",
]

valeur_minimale = pd.Series([
    9046,
    86793,
    243872,
    1400,
    66850,
    273993,
], index=instances, name="MinVal")

resultats = pd.read_csv("resultats_calibration.txt")

# Ajouter la valeur minimale connue correspondant à chaque instance
resultats = resultats.join(valeur_minimale, on="Nom")

# Ajout écart absolu et relatif
resultats["EcartAbs"] = resultats["FctObjFinale"] - resultats["MinVal"]
resultats["EcartRel"] = resultats["EcartAbs"] / resultats["MinVal"]

# Grouper selon les combinaisons de paramètres
res_groups = resultats.groupby(["TempInit", "NbVoisins", "Alpha", "NbPalier"])[["EcartRel", "EcartAbs"]].mean()
res_ecart_rel = res_groups.sort_values("EcartRel")
res_ecart_abs = res_groups.sort_values("EcartAbs")

# Afficher les résultats totaux selon le classement
resultats_sort = resultats.set_index(["TempInit", "NbVoisins", "Alpha", "NbPalier", "Nom"]).sort_index()

#resultats_sort.loc[res_ecart_rel.index, ["FctObjFinale", "MinVal", "EcartRel", "EcartAbs"]]

best_resultats = resultats_sort.loc[res_ecart_rel.index[0], ["FctObjFinale", "MinVal", "EcartRel", "EcartAbs"]]