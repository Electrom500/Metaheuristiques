import numpy as np
import pandas as pd

import matplotlib.pyplot as plt

plt.close("all")
plt.rcParams["axes.autolimit_mode"] = "round_numbers"

########################################################################################
# Lecture des fichiers d'entrée

instances = [
    "wt100-56.txt",
    "wt100-61.txt",
    "wt100-66.txt",
    "wt100-81.txt",
    "wt100-86.txt",
    "wt100-94.txt",
]

valeur_minimale = pd.Series(
    [
        9046,
        86793,
        243872,
        1400,
        66850,
        273993,
    ],
    index=instances,
    name="MinVal",
)

fichiers_resultats = [
    "resultats_calibration.txt",
    "resultats_calibration_63501.txt",
    "resultats_calibration_63502.txt",
    "resultats_calibration_63503.txt",
    "resultats_calibration_63504.txt",
    "resultats_calibration_884.txt",
    "resultats_calibration_15324.txt",
    "resultats_calibration_17672.txt",
    "resultats_calibration_32704.txt",
]

resultats = pd.concat([pd.read_csv(f) for f in fichiers_resultats], axis=0)

# Ajouter la valeur minimale connue correspondant à chaque instance
resultats = resultats.join(valeur_minimale, on="Nom")

# Ajout écart absolu et relatif
resultats["EcartAbs"] = resultats["FctObjFinale"] - resultats["MinVal"]
resultats["EcartRel"] = resultats["EcartAbs"] / resultats["MinVal"]

########################################################################################
# Grouper selon les combinaisons de paramètres
params = ["TempInit", "NbVoisins", "Alpha", "NbPalier"]

res_groups = resultats.groupby(params)[["EcartRel", "EcartAbs"]].mean()
res_ecart_rel = res_groups.sort_values("EcartRel")
res_ecart_abs = res_groups.sort_values("EcartAbs")

# Afficher les résultats totaux selon le classement
resultats_sort = resultats.set_index(params + ["Nom"]).sort_index()

# resultats_sort.loc[res_ecart_rel.index, ["FctObjFinale", "MinVal", "EcartRel", "EcartAbs"]]

best_resultats = resultats_sort.loc[
    res_ecart_rel.index[0], ["FctObjFinale", "MinVal", "EcartRel", "EcartAbs"]
]

########################################################################################
# Affichage des valeurs des paramètres pour les meilleures combinaisons
best_combinaisons = res_ecart_rel.reset_index().iloc[:100]
param_labels = [
    "Température initiale",
    "Nombre de voisins",
    "Alpha",
    "Nombre de paliers",
]
param_widths = [50, 2, 0.03, 2]

fig, axes = plt.subplots(2, 2, layout="constrained")

for i, p in enumerate(params):
    c = "tab:blue"
    ax = axes.flatten()[i]
    counts = best_combinaisons[p].value_counts().sort_index()
    ax.bar(
        counts.index,
        counts,
        width=param_widths[i],
        edgecolor="tab:blue",
        facecolor=("tab:blue", 0.5),
    )

    ax.set_xlabel(param_labels[i])
    ax.set_ylabel("Nombre d'occurrences")

    if i == 0:
        ax.set_xlim(0, 5000)
        #ax.set_xticks(counts.index, labels=counts.index, rotation=45, ha="right")
    else:
        ax.set_xticks(counts.index)

fig.savefig("param_distributions.png", dpi=300)
