/* Exercise 2: Parallel sum of 1..10,000,000 using MPI.
 * Each process sums its own block of the range, then MPI_Reduce adds the
 * partial sums on rank 0.
 * Build: mpicc -O2 -o ex2_sum ex2_sum.c
 * Run:   mpirun -np 4 ./ex2_sum
 */
#include <mpi.h>
#include <stdio.h>

#define N 10000000LL

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    /* Split 1..N into `size` nearly equal blocks. */
    long long base = N / size, extra = N % size;
    long long lo = rank * base + (rank < extra ? rank : extra) + 1;
    long long hi = lo + base + (rank < extra ? 1 : 0) - 1;

    long long local = 0;
    for (long long i = lo; i <= hi; i++) local += i;

    long long total = 0;
    MPI_Reduce(&local, &total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double elapsed = MPI_Wtime() - start;

    if (rank == 0) {
        long long expected = N * (N + 1) / 2;
        printf("procs=%d sum=%lld expected=%lld %s time=%.6f\n", size, total,
               expected, total == expected ? "OK" : "WRONG", elapsed);
    }
    MPI_Finalize();
    return 0;
}
