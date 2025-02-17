import pathlib
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

plt.close("all")
plt.rcParams["axes.autolimit_mode"] = "round_numbers"

# Import results
files = [
    "resultats_wt100-56.csv",
    "resultats_wt100-61.csv",
    "resultats_wt100-66.csv",
    "resultats_wt100-81.csv",
    "resultats_wt100-86.csv",
    "resultats_wt100-94.csv",
    "resultats_wt40-61.csv",
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

offsets = [-0.3, 0, 0.3]
methode_labels = ["Insertion", "Permutation", "Voisinage orienté"]
colors = ["tab:blue", "tab:orange", "tab:green"]
voisinages = [1, 2, 5, 10, 20]

for file, min_val in zip(files, valeur_minimale):
    df = pd.read_csv(file)

    fig, ax = plt.subplots(1, 1)
    ax.hlines(
        1e-3 * min_val,
        -1,
        len(voisinages) + 1,
        color="0.7",
        ls="--",
        lw=1,
        label="Meilleure solution connue",
    )

    for methode, (off, c, label) in enumerate(zip(offsets, colors, methode_labels)):
        for pos, nb_voisins in enumerate(voisinages):
            subset = df.loc[
                (df["MethodeVoisinage"] == methode) & (df["NbVoisins"] == nb_voisins),
                "FctObjFinale",
            ]

            ax.boxplot(
                1e-3 * subset,
                positions=[pos + off],
                vert=True,
                whis=(0, 100),
                showfliers=False,
                patch_artist=True,
                boxprops=dict(facecolor=(c, 0.3), edgecolor=c, lw=1),
                medianprops=dict(color=c, lw=1),
                whiskerprops=dict(color=c, lw=1),
                capprops=dict(color=c, lw=1),
                label=label if pos == 0 else None,
                widths=0.15,
            )

    ax.set_xticks(range(len(voisinages)), labels=voisinages)
    ax.set_xlim(-0.5, len(voisinages) - 0.5)
    ax.set_xlabel("Taille du voisinage")
    # ax.set_ylim(0, None)
    ax.set_ylim(max(0, ax.get_ylim()[0]), None)

    ax.set_ylabel("Valeur optimale de la fonction objectif (x 1000)")
    ax.set_title(file)
    ax.legend(loc="best")

    fig.savefig(pathlib.Path(file).with_suffix(".png"), dpi=300)
    plt.close(fig)

    subset = df.loc[
        (df["MethodeVoisinage"] == 0) & (df["NbVoisins"] == 5),
        ["FctObjFinale", "EvalPourTrouver"],
    ]
    subset["EcartOptimal"] = 100 * np.abs(subset["FctObjFinale"] - min_val) / min_val

    print(file)
    print(subset[["FctObjFinale", "EcartOptimal", "EvalPourTrouver"]].head(5))
    print()
    print(subset[["FctObjFinale", "EcartOptimal", "EvalPourTrouver"]].describe())
    print()
