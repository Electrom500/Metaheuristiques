import concurrent.futures
import multiprocessing as mp
import pathlib
import subprocess
import itertools
import os

import numpy as np
import pandas as pd

TaillePop = np.linspace(10, 1010, 26, dtype=int)
ProbCr = np.array([0.2, 0.4, 0.6, 0.8, 1.0, 4.0, 7.0, 10.0])
ProbMut = np.linspace(0, 1, 6)

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

NbRepetitions = 10


def execute(args):
    pop, pcr, pmut, i = args
    process_id = os.getpid()
    for _ in range(NbRepetitions):
        os.system(
            f"dist/release/LaboAE {i} {pop} {pcr} {pmut} 50000 resultats_calibration_{process_id}.csv 0"
        )

    return 0


combinaisons = list(
    itertools.product(TaillePop, ProbCr, ProbMut, instances)
)

# Exécution en parallèle
with concurrent.futures.ProcessPoolExecutor(max_workers=16) as executor:
    res = list(executor.map(execute, combinaisons))
