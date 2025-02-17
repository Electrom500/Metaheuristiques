import concurrent.futures
import multiprocessing as mp
import pathlib
import subprocess
import itertools
import os

import numpy as np
import pandas as pd

TempInit = np.linspace(100, 5000, 41, dtype=int)[1:14]  # Maxime
# TempInit = np.linspace(100, 5000, 41, dtype=int)[14:28] # Damien
# TempInit = np.linspace(100, 5000, 41, dtype=int)[28:] # Niels

Alpha = np.array([0.75, 0.8, 0.85, 0.9, 0.95])
TailleVoisinage = np.array([2, 5, 10, 15, 20])
NbPalier = np.array([5, 10, 15, 20, 50])

instances = np.array(
    [
        "wt100-56.txt",
        "wt100-61.txt",
        "wt100-66.txt",
        "wt100-81.txt",
        "wt100-86.txt",
        "wt100-94.txt",
    ],
    dtype=object,
)

NbRepetitions = 10
NbCombinaisons = (
    len(TempInit)
    * len(Alpha)
    * len(TailleVoisinage)
    * len(NbPalier)
    * len(instances)
    * NbRepetitions
)


def execute(args):
    T, v, a, p, i = args
    process_id = os.getpid()
    for _ in range(NbRepetitions):
        os.system(
            f"dist/release/recuit {i} {v} {T} {a} {p} 100000 resultats_calibration_{process_id}.txt"
        )

    return 0


combinaisons = list(
    itertools.product(TempInit, TailleVoisinage, Alpha, NbPalier, instances)
)

# Exécution en parallèle
with concurrent.futures.ProcessPoolExecutor(max_workers=4) as executor:
    res = list(executor.map(execute, combinaisons))
