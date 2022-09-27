#include <stdio.h>
#include <mpi.h>

int main(void)
{
    const unsigned int sz = 128;
    char buffer[sz];
    int size, rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0)
    {
        printf("Loading file in process %i.\n", rank);
        // load text file
        FILE *fp = fopen("test_files/hello.html", "r");
        fread(buffer, sz, 1, fp);
        fclose(fp);
        // send to other processes
        for (int i = 1; i < size; i++)
        {
            MPI_Send(buffer, sz, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(buffer, sz, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received: %s", rank, buffer);
    }

    MPI_Finalize();
    return 0;
}
