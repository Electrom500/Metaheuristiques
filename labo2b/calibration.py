import concurrent.futures
import pathlib
import subprocess
import os

import numpy as np
import pandas as pd

TempInit = np.linspace(100, 5000, 41)
Alpha = np.array([0.75, 0.8, 0.85, 0.9, 0.95])
TailleVoisinage = np.array([2, 5, 10, 15, 20])
NbPalier = np.array([5, 10, 15, 20, 50])

instances = [
    "wt100-56.txt",
    "wt100-61.txt",
    "wt100-66.txt",
    "wt100-81.txt",
    "wt100-86.txt",
    "wt100-94.txt",
]

NbRepetitions = 10
NbCombinaisons = len(TempInit) * len(Alpha) * len(TailleVoisinage) * len(NbPalier) * len(instances) * NbRepetitions

for T in TempInit:
    for a in Alpha:
        for v in TailleVoisinage:
            for p in NbPalier:
                for i in instances:
                    for _ in range(NbRepetitions):
                        os.system(
                            f"dist/release/recuit {i} {v} {T} {a} {p} 100000 resultats_calibration.txt"
                        )