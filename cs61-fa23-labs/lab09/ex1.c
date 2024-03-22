#include <mpi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define TERMINATE -1
#define READY 0

void hello_world(int message) {
    int procID;

    // get the ID of the current program, and store in `procID`
    MPI_Comm_rank(MPI_COMM_WORLD, &procID);

    // Sleep for message % 5 seconds
    sleep(message % 5);

    // Print message
    printf("Hello world from task %d with process %d\n", message, procID);
}

int main(int argc, char** argv) {
    int numTasks = atoi(argv[1]);


    // use MPI_Init to initialize the program
    MPI_Init(&argc, &argv);

    int procID, totalProcs;
    // get the total number of processes and store in `totalProcs`
    MPI_Comm_size(MPI_COMM_WORLD, &totalProcs);

    // get the ID of the current program, and store in `procID`
    MPI_Comm_rank(MPI_COMM_WORLD, &procID);

    // check if the current process is the manager
    if (procID == 0) {
        // Manager node
        int nextTask = 0;
        MPI_Status status;
        int32_t message;

        // loop until we've completed `numTasks`
        while (nextTask < numTasks) {
            // receive a message from any source (so we know that this node is done with its task)
            MPI_Recv(&message, 1, MPI_INT32_T, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            assert(message == READY);

            // get the source process using the `status` struct
            int src = status.MPI_SOURCE;

            // send `nextTask` as the message to the process we just received a message from
            message = nextTask;
            MPI_Send(&message, 1, MPI_INT32_T, src, 0, MPI_COMM_WORLD);

            // increment `nextTask` by 1
            nextTask++;
        }

        // Wait for all processes to finish
        // loop through all processes
        // Hint: we have `totalProcs - 1` total, since there's a manager node
        for (int i = 0; i < totalProcs - 1; i++) {
            // receive a message from any source (so we know that this node is done with its task)
            MPI_Recv(&message, 1, MPI_INT32_T, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            assert(message == READY);

            // get the source process using the `status` struct
            int src = status.MPI_SOURCE;

            // send `TERMINATE` as the message to the process we just received a message from
            message = TERMINATE;
            MPI_Send(&message, 1, MPI_INT32_T, src, 0, MPI_COMM_WORLD);
        }
    } else {
        // Worker node
        int32_t message;

        while (true) {
            // let the manager node know that this worker is ready
            // Hint: use MPI_Send to send a message
            message = READY;
            MPI_Send(&message, 1, MPI_INT32_T, 0, 0, MPI_COMM_WORLD);

            // receive 1 message from the manager and store it in `message`
            // Hint: use MPI_Recv to receive a message
            MPI_Recv(&message, 1, MPI_INT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // if the `message` is TERMINATE, break out of the loop to terminate
            if (message == TERMINATE) {
                break;
            }

            // call `hello_world` and pass `message` as the argument
            hello_world(message);
        }
    }

    // call MPI_Finalize since it is the end of the program
    MPI_Finalize();
}
