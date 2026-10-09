/* Exercise 5.1: source and destination do not match.
 * Rank 0 sends to rank 1, but rank 1 waits for a message from rank 2.
 * Rank 2 never sends anything, so rank 1's MPI_Recv blocks forever (hang).
 * Run with exactly 3 processes, and use a timeout so it can be stopped:
 *   mpirun --oversubscribe -np 3 timeout 5 ./ex5a_mismatch
 */
#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size, value = 42;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 3) {
        if (rank == 0) printf("Run with at least 3 processes.\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        printf("Rank 0: sending %d to rank 1\n", value);
        fflush(stdout);
        MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("Rank 0: send finished\n");
    } else if (rank == 1) {
        int got;
        printf("Rank 1: waiting for a message from rank 2 (it will never come)\n");
        fflush(stdout);
        MPI_Recv(&got, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Rank 1: received %d\n", got); /* never reached */
    }
    /* rank 2 does nothing */

    MPI_Finalize();
    return 0;
}
