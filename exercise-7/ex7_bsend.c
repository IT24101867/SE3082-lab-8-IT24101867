/* Exercise 7: Exercise 6 rewritten with Buffered Send (MPI_Bsend).
 * Workers hand their count to MPI_Bsend (copied into an attached buffer, so
 * the send returns at once); rank 0 still receives with MPI_ANY_SOURCE.
 * Build: mpicc -O2 -o ex7_bsend ex7_bsend.c
 * Run:   mpirun -np 4 ./ex7_bsend
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
        int bufsize = (int)sizeof(long long) + MPI_BSEND_OVERHEAD;
        void *buf = malloc(bufsize);
        MPI_Buffer_attach(buf, bufsize);
        MPI_Bsend(&inside, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
        MPI_Buffer_detach(&buf, &bufsize); /* waits until delivered */
        free(buf);
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
