/* Exercise 3: Monte Carlo estimate of Pi using MPI (10,000,000 trials).
 * Each process throws its share of random points into the unit square and
 * counts how many land inside the quarter circle. Workers send their count
 * to rank 0 with MPI_Send; rank 0 receives from each rank in order
 * (explicit source) and computes Pi = 4 * inside / total.
 * Build: mpicc -O2 -o ex3_pi ex3_pi.c
 * Run:   mpirun -np 4 ./ex3_pi
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TRIALS 10000000LL

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    long long mine = TRIALS / size + (rank < TRIALS % size ? 1 : 0);
    unsigned int seed = 12345u + 7919u * (unsigned int)rank;
    long long inside = 0;
    for (long long i = 0; i < mine; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0) inside++;
    }

    long long total_inside = inside;
    if (rank == 0) {
        for (int src = 1; src < size; src++) {
            long long c;
            MPI_Recv(&c, 1, MPI_LONG_LONG, src, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            total_inside += c;
        }
    } else {
        MPI_Send(&inside, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double elapsed = MPI_Wtime() - start;

    if (rank == 0) {
        double pi = 4.0 * (double)total_inside / (double)TRIALS;
        printf("procs=%d trials=%lld pi=%.6f time=%.6f\n", size, TRIALS, pi,
               elapsed);
    }
    MPI_Finalize();
    return 0;
}
