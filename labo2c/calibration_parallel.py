import concurrent.futures
import multiprocessing as mp
import pathlib
import subprocess
import itertools
import os

import numpy as np
import pandas as pd

LngListeTabous = np.array([1, 2, 4, 7, 10, 15, 20])

TailleVoisinage = np.array([2, 5, 10, 15, 20])

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


def execute(args):
    v, L, i = args
    process_id = os.getpid()
    for _ in range(NbRepetitions):
        os.system(
            f"dist/release/tabou {i} {v} {L} 100000 resultats_calibration_{process_id}.txt"
        )

    return 0


combinaisons = list(
    itertools.product(TailleVoisinage, LngListeTabous, instances)
)

# Exécution en parallèle
with concurrent.futures.ProcessPoolExecutor(max_workers=4) as executor:
    res = list(executor.map(execute, combinaisons))
