/* Exercise 6: Exercise 3 (Monte Carlo Pi) with MPI_ANY_SOURCE on the receive.
 * Rank 0 now accepts the workers' counts in whatever order they arrive,
 * instead of waiting for rank 1, then rank 2, and so on. It prints the order
 * so it can be compared with Exercise 3.
 * Build: mpicc -O2 -o ex6_anysource ex6_anysource.c
 * Run:   mpirun -np 4 ./ex6_anysource
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
    int order[64], n = 0;
    if (rank == 0) {
        for (int k = 1; k < size; k++) {
            long long c;
            MPI_Status st;
            MPI_Recv(&c, 1, MPI_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
                     &st);
            total_inside += c;
            if (n < 64) order[n++] = st.MPI_SOURCE;
        }
    } else {
        MPI_Send(&inside, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double elapsed = MPI_Wtime() - start;

    if (rank == 0) {
        double pi = 4.0 * (double)total_inside / (double)TRIALS;
        printf("procs=%d pi=%.6f time=%.6f arrival order:", size, pi, elapsed);
        for (int i = 0; i < n; i++) printf(" %d", order[i]);
        printf("\n");
    }
    MPI_Finalize();
    return 0;
}
