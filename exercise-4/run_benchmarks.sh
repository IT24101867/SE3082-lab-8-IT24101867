#!/usr/bin/env bash
# Runs Exercise 2 (sum) and Exercise 3 (Pi) for several process counts,
# REPS times each, and writes raw timings to results/*.csv.
# Usage: ./run_benchmarks.sh
set -e
cd "$(dirname "$0")"
mkdir -p bin results
for f in ../exercise-2/ex2_sum.c ../exercise-3/ex3_pi.c; do
  mpicc -O2 -o "bin/$(basename "$f" .c)" "$f"
done

PROCS="1 2 3 4 6 8"
REPS=7
RUN="mpirun --oversubscribe"

for prog in ex2_sum ex3_pi; do
  out="results/${prog}.csv"
  echo "procs,rep,time" > "$out"
  for p in $PROCS; do
    for r in $(seq 1 $REPS); do
      t=$($RUN -np "$p" "bin/$prog" | sed -n 's/.*time=\([0-9.]*\).*/\1/p')
      echo "$p,$r,$t" >> "$out"
    done
  done
  echo "wrote $out"
done
