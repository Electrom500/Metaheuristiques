import concurrent.futures
import itertools
import os

import numpy as np
import pandas as pd

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

NbRepetitions = 250


def execute(instance):
    for _ in range(NbRepetitions):
        os.system(
            f"dist/release/LaboAE {instance} 250 0.2 0.2 50000 resultats_ae_{instance[:-4]}.csv 0"
        )

    return 0

# Exécution en parallèle
with concurrent.futures.ProcessPoolExecutor(max_workers=5) as executor:
    res = list(executor.map(execute, instances))
