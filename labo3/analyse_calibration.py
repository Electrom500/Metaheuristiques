import numpy as np
import pandas as pd

import matplotlib.pyplot as plt

plt.close("all")
plt.rcParams["axes.autolimit_mode"] = "round_numbers"

########################################################################################
# Lecture des fichiers d'entrée

instances = np.array(
    [
        "SOP18.txt",
        "SOP42.txt",
        "SOP48.txt",
        "SOP50.txt",
        "SOP63.txt",
    ],
    dtype=object,
)

valeur_minimale = pd.Series(
    [
        55,
        243,
        351,
        467,
        62,
    ],
    index=instances,
    name="MinVal",
)

fichiers_resultats = [f"resultats_calibration_{6874+i}.csv" for i in range(0, 16)]

resultats = pd.concat([pd.read_csv(f) for f in fichiers_resultats], axis=0)

# Ajouter la valeur minimale connue correspondant à chaque instance
resultats = resultats.join(valeur_minimale, on="Nom")

# Ajout écart absolu et relatif
resultats["EcartAbs"] = resultats["FctObjFinale"] - resultats["MinVal"]
resultats["EcartRel"] = resultats["EcartAbs"] / resultats["MinVal"]
resultats["EcartQuad"] = resultats["EcartAbs"]**2

########################################################################################
# Grouper selon les combinaisons de paramètres
params = ["TaillePop", "ProbCr", "ProbMut"]

res_groups = resultats.groupby(params)[["EcartRel", "EcartAbs", "EcartQuad"]].mean()
res_ecart_rel = res_groups.sort_values("EcartRel")
res_ecart_abs = res_groups.sort_values("EcartAbs")
res_ecart_quad = res_groups.sort_values("EcartQuad")

# Afficher les résultats totaux selon le classement
resultats_sort = resultats.set_index(params + ["Nom"]).sort_index()

# resultats_sort.loc[res_ecart_rel.index, ["FctObjFinale", "MinVal", "EcartRel", "EcartAbs"]]

best_resultats = resultats_sort.loc[
    res_ecart_quad.index[1], ["FctObjFinale", "MinVal", "EcartRel", "EcartAbs", "EcartQuad"]
]

########################################################################################
# Affichage des valeurs des paramètres pour les meilleures combinaisons
best_combinaisons = res_ecart_quad.reset_index().iloc[:75]
param_labels = [
    "Taille de population",
    "Proportion de croisements",
    "Probabilité de mutation",
]
param_widths = [20, 0.1, 0.1]

param_xlim = [
    (0, 1100),
    (0, 11),
    (-0.1, 1.1),
]

fig, axes = plt.subplot_mosaic("AB;C.", layout="constrained")

for i, (p, ax) in enumerate(zip(params, axes.values())):
    c = "tab:blue"
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

    ax.set_xlim(*param_xlim[i])

    if i == 2:
        ax.set_xticks(np.linspace(0, 1, 6))

fig.savefig("param_distributions.png", dpi=300)
