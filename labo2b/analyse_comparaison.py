import pathlib
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

pd.options.mode.copy_on_write = True
plt.close("all")
plt.rcParams["axes.autolimit_mode"] = "round_numbers"

# Import results
files = [
    "wt100-56.txt",
    "wt100-61.txt",
    "wt100-66.txt",
    "wt100-81.txt",
    "wt100-86.txt",
    "wt100-94.txt",
]

valeur_minimale = [
    9046,
    86793,
    243872,
    1400,
    66850,
    273993,
    20281,
]

res_descente = pd.read_csv("../labo1/resultats_descente_labo2.csv")
res_recuit_rel = pd.read_csv("resultats_recuit_meilleur_relatif.csv")
res_recuit_abs = pd.read_csv("resultats_recuit_meilleur_absolu.csv")
res_tabou = pd.read_csv("../labo2c/resultats_tabou_meilleur_absolu.csv")

resultats = [
    res_descente,
    res_recuit_rel,
    res_recuit_abs,
    res_tabou,
]

labels = [
    "Descente",
    "Recuit simulé\n(écart relatif)",
    "Recuit simulé\n(écart absolu)",
    "Recherche avec\ntabous",
]

colors = ["tab:blue", "tab:orange", "tab:green", "tab:red"]

for file, min_val in zip(files, valeur_minimale):
    fig, ax = plt.subplots(1, 1)
    ax.hlines(
        1e-3 * min_val,
        -1,
        5,
        color="0.7",
        ls="--",
        lw=1,
        label="Meilleure solution connue",
    )

    for i, res in enumerate(resultats):
        subset = res.query(f"Nom == '{file}'")
        c = colors[i]

        ax.boxplot(
            1e-3 * subset["FctObjFinale"],
            positions=[i],
            vert=True,
            whis=(0, 100),
            showfliers=False,
            patch_artist=True,
            boxprops=dict(facecolor=(c, 0.3), edgecolor=c, lw=1),
            medianprops=dict(color=c, lw=1),
            whiskerprops=dict(color=c, lw=1),
            capprops=dict(color=c, lw=1),
            #label=labels[i],
            widths=0.5,
        )

        subset["EcartOptimal"] = 100 * np.abs(subset["FctObjFinale"] - min_val) / min_val

        if i > 1:
            print(labels[i])
            print(file)
            print(subset[["FctObjFinale", "EcartOptimal", "EvalPourTrouver"]].head(5))
            print()
            print(subset[["FctObjFinale", "EcartOptimal", "EvalPourTrouver"]].describe())
            print()

    ax.set_xticks(range(0, len(resultats)), labels=labels)

    #ax.set_xticks(range(len(voisinages)), labels=voisinages)
    ax.set_xlim(-0.5, len(resultats) - 0.5)
    #ax.set_xlabel("Taille du voisinage")
    # ax.set_ylim(0, None)
    ax.set_ylim(max(0, ax.get_ylim()[0]), None)

    ax.set_ylabel("Valeur de la fonction objectif finale (x 1000)")
    ax.set_title(file)
    #ax.legend(loc="best")

    fig.savefig(pathlib.Path(file).with_suffix(".png"), dpi=300)
    plt.close(fig)
