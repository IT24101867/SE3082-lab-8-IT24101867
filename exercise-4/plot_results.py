"""Exercise 4: plot Time vs Number of Processors and Speedup.
Reads results/ex2_sum.csv and results/ex3_pi.csv (from run_benchmarks.sh),
uses the median of the repeated runs, and saves PNG graphs to results/.
"""
import csv
import statistics
from collections import defaultdict

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

PROGRAMS = [("ex2_sum", "Exercise 2: Sum of 1..10,000,000"),
            ("ex3_pi", "Exercise 3: Monte Carlo Pi (10,000,000 trials)")]


def load(name):
    runs = defaultdict(list)
    with open(f"results/{name}.csv") as f:
        for row in csv.DictReader(f):
            runs[int(row["procs"])].append(float(row["time"]))
    procs = sorted(runs)
    med = [statistics.median(runs[p]) for p in procs]
    return procs, med


summary = []
for name, title in PROGRAMS:
    procs, med = load(name)
    speedup = [med[0] / t for t in med]

    plt.figure(figsize=(6, 4))
    plt.plot(procs, med, "o-")
    plt.xlabel("Number of processors")
    plt.ylabel("Time (s, median of runs)")
    plt.title(f"{title}\nTime vs Processors")
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(f"results/{name}_time.png", dpi=150)
    plt.close()

    plt.figure(figsize=(6, 4))
    plt.plot(procs, speedup, "o-", label="Measured speedup")
    plt.plot(procs, procs, "--", color="gray", label="Ideal (linear)")
    plt.xlabel("Number of processors")
    plt.ylabel("Speedup  (T1 / Tp)")
    plt.title(f"{title}\nSpeedup")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(f"results/{name}_speedup.png", dpi=150)
    plt.close()

    for p, t, s in zip(procs, med, speedup):
        summary.append((name, p, t, s))

with open("results/summary.csv", "w") as f:
    f.write("program,procs,median_time_s,speedup\n")
    for name, p, t, s in summary:
        f.write(f"{name},{p},{t:.6f},{s:.3f}\n")
print(open("results/summary.csv").read())
