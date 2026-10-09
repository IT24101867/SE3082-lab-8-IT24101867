/* Exercise 5.2: message passing with Buffered Send (MPI_Bsend).
 * Rank 0 sends an array to rank 1 using MPI_Bsend. The data is copied into a
 * user-attached buffer, so the send returns immediately. The send variable
 * (`data`) is never overwritten, and rank 1 receives into its own separate
 * variable (`recv_data`).
 * Build: mpicc -O2 -o ex5b_bsend ex5b_bsend.c
 * Run:   mpirun -np 2 ./ex5b_bsend
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT 5

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) printf("Run with at least 2 processes.\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        int data[COUNT] = {10, 20, 30, 40, 50};

        int bufsize = COUNT * (int)sizeof(int) + MPI_BSEND_OVERHEAD;
        void *buf = malloc(bufsize);
        MPI_Buffer_attach(buf, bufsize);

        MPI_Bsend(data, COUNT, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("Rank 0: Bsend done, data[0..%d] unchanged:", COUNT - 1);
        for (int i = 0; i < COUNT; i++) printf(" %d", data[i]);
        printf("\n");

        /* Detach blocks until the buffered message has been delivered. */
        MPI_Buffer_detach(&buf, &bufsize);
        free(buf);
    } else if (rank == 1) {
        int recv_data[COUNT];
        MPI_Recv(recv_data, COUNT, MPI_INT, 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        printf("Rank 1: received:");
        for (int i = 0; i < COUNT; i++) printf(" %d", recv_data[i]);
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
