import pathlib
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

pd.options.mode.copy_on_write = True
plt.close("all")
plt.rcParams["axes.autolimit_mode"] = "round_numbers"

# Import results
files = [
    "SOP18.txt",
    "SOP42.txt",
    "SOP48.txt",
    "SOP50.txt",
    "SOP63.txt",
]

valeur_minimale = np.array(
    [
        55,
        243,
        351,
        467,
        62
    ]
)

colors = ["tab:blue", "tab:orange", "tab:green", "tab:red", "tab:purple"]

########################################################################################
# Figure en "absolu"
fig, ax = plt.subplots(1, 1)

ax.plot(range(len(valeur_minimale)), valeur_minimale, marker="o", ls="", color="k", label="Meilleure solution connue")

for i, (file, min_val) in enumerate(zip(files, valeur_minimale)):
    df = pd.read_csv(f"resultats_ae_{file[:-4]}.csv")
    df["EcartOptimal"] = 100 * np.abs(df["FctObjFinale"] - min_val) / min_val

    c = colors[i]

    ax.boxplot(
        df["FctObjFinale"],
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

    print(file)
    print(df[["FctObjFinale", "EcartOptimal", "EvalPourTrouver"]].head(5))
    print()
    print(df[["FctObjFinale", "EcartOptimal", "EvalPourTrouver"]].describe())
    print()

ax.set_xticks(range(0, len(valeur_minimale)), labels=[f[:-4] for f in files])

#ax.set_xticks(range(len(voisinages)), labels=voisinages)
ax.set_xlim(-0.5, len(files) - 0.5)
ax.set_xlabel("Instance du problème")
# ax.set_ylim(0, None)
ax.set_ylim(max(0, ax.get_ylim()[0]), None)

ax.set_ylabel("Valeur de la fonction objectif finale")
ax.legend(loc="best")

fig.savefig("resultats.png", dpi=300)
plt.close(fig)


########################################################################################
# Figure en "absolu"
fig, ax = plt.subplots(1, 1)

optim = pd.read_csv("optim.csv")

for i, (file, min_val) in enumerate(zip(files, valeur_minimale)):
    df = pd.read_csv(f"resultats_ae_{file[:-4]}.csv")

    c = "tab:blue"

    ax.boxplot(
        df["FctObjFinale"],
        positions=[i],
        vert=True,
        whis=(0, 100),
        showfliers=False,
        patch_artist=True,
        boxprops=dict(facecolor=(c, 0.3), edgecolor=c, lw=1),
        medianprops=dict(color=c, lw=1),
        whiskerprops=dict(color=c, lw=1),
        capprops=dict(color=c, lw=1),
        label="(250, 0.2, 0.2), Max Eval = 50 000" if i == 0 else None,
        widths=0.5,
    )

ax.plot(
    range(1, len(valeur_minimale)),
    optim.query("TaillePop == 250").sort_values("Nom")["FctObjFinale"],
    marker="d",
    color="tab:orange",
    ls="",
    alpha=0.7,
    label="(250, 0.2, 0.2), Max Eval = 1 000 000"
)

ax.plot(
    range(1, len(valeur_minimale)),
    optim.query("TaillePop == 1000").sort_values("Nom")["FctObjFinale"],
    marker="s",
    color="tab:red",
    ls="",
    alpha=0.7,
    label="(1000, 10, 0.2), Max Eval = 1 000 000"
)

ax.plot(
    range(len(valeur_minimale)),
    valeur_minimale,
    marker="o",
    ls="",
    color="k",
    label="Meilleure solution connue"
)

ax.set_xticks(range(0, len(valeur_minimale)), labels=[f[:-4] for f in files])

#ax.set_xticks(range(len(voisinages)), labels=voisinages)
ax.set_xlim(-0.5, len(files) - 0.5)
ax.set_xlabel("Instance du problème")
# ax.set_ylim(0, None)
ax.set_ylim(max(0, ax.get_ylim()[0]), 1000)

ax.set_ylabel("Valeur de la fonction objectif finale")
ax.legend(loc="best")

fig.savefig("resultats_optim.png", dpi=300)
plt.close(fig)

########################################################################################
# Figure en "relatif"

fig, ax = plt.subplots(1, 1)

for i, (file, min_val) in enumerate(zip(files, valeur_minimale)):
    df = pd.read_csv(f"resultats_ae_{file[:-4]}.csv")

    c = colors[i]

    ax.boxplot(
        100 * (df["FctObjFinale"] - min_val) / min_val,
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

ax.set_xticks(range(0, len(valeur_minimale)), labels=[f[:-4] for f in files])

#ax.set_xticks(range(len(voisinages)), labels=voisinages)
ax.set_xlim(-0.5, len(files) - 0.5)
ax.set_xlabel("Instance du problème")
# ax.set_ylim(0, None)
ax.set_ylim(max(0, ax.get_ylim()[0]), None)

ax.set_ylabel("Écart relatif par rapport à la meilleure\nsolution connue (%)")

fig.savefig("resultats_relatif.png", dpi=300)
plt.close(fig)

########################################################################################
# Figure optim en "relatif"

fig, ax = plt.subplots(1, 1)

for i, (file, min_val) in enumerate(zip(files, valeur_minimale)):
    df = pd.read_csv(f"resultats_ae_{file[:-4]}.csv")

    c = "tab:blue"

    ax.boxplot(
        100 * (df["FctObjFinale"] - min_val) / min_val,
        positions=[i],
        vert=True,
        whis=(0, 100),
        showfliers=False,
        patch_artist=True,
        boxprops=dict(facecolor=(c, 0.3), edgecolor=c, lw=1),
        medianprops=dict(color=c, lw=1),
        whiskerprops=dict(color=c, lw=1),
        capprops=dict(color=c, lw=1),
        label="(250, 0.2, 0.2), Max Eval = 50 000" if i == 0 else None,
        widths=0.5,
    )

ax.plot(
    range(1, len(valeur_minimale)),
    100 * (optim.query("TaillePop == 250").sort_values("Nom")["FctObjFinale"] - valeur_minimale[1:]) / valeur_minimale[1:],
    marker="d",
    color="tab:orange",
    ls="",
    alpha=0.7,
    label="(250, 0.2, 0.2), Max Eval = 1 000 000"
)

ax.plot(
    range(1, len(valeur_minimale)),
    100 * (optim.query("TaillePop == 1000").sort_values("Nom")["FctObjFinale"] - valeur_minimale[1:]) / valeur_minimale[1:],
    marker="s",
    color="tab:red",
    alpha=0.7,
    ls="",
    label="(1000, 10, 0.2), Max Eval = 1 000 000"
)

ax.set_xticks(range(0, len(valeur_minimale)), labels=[f[:-4] for f in files])

#ax.set_xticks(range(len(voisinages)), labels=voisinages)
ax.set_xlim(-0.5, len(files) - 0.5)
ax.set_xlabel("Instance du problème")
# ax.set_ylim(0, None)
ax.set_ylim(max(0, ax.get_ylim()[0]), None)
ax.legend(loc="best")

ax.set_ylabel("Écart relatif par rapport à la meilleure\nsolution connue (%)")

fig.savefig("resultats_optim_relatif.png", dpi=300)
plt.close(fig)
