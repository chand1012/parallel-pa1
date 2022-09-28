#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>

#define ARRAY_SIZE 16000

int randint(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

int factorial(int n)
{
    if (n == 0)
    {
        return 1;
    }
    else
    {
        return n * factorial(n - 1);
    }
}

long process_chunk(int *chunk, int chunk_size, int *result)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < chunk_size; i++)
    {
        result[i] = factorial(chunk[i]);
    }
    // get the time after running the factorial function
    clock_gettime(CLOCK_MONOTONIC, &end);
    // return the runtime
    return ((end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec)) / 1000;
}

int main(void)
{
    srand(time(NULL));
    int m, n;
    int rank, size;
    struct timespec start, end;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (ARRAY_SIZE % size != 0)
    {
        if (rank == 0)
        {
            printf("The array size must be divisible by the number of processes.\n");
        }
        MPI_Finalize();
        return 0;
    }

    if (rank == 0)
    {
        printf("Enter the minimum and maximum number separated by a space: \n");
        scanf("%d %d", &m, &n);
        clock_gettime(CLOCK_MONOTONIC, &start);
        int testArray[ARRAY_SIZE];
        int testResult[ARRAY_SIZE];

        // generate the random numbers
        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            testArray[i] = randint(m, n);
        }

        // divide the array into chunks
        int chunkSize = ARRAY_SIZE / size;
        int chunk[chunkSize];
        int localChunk[chunkSize];

        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < chunkSize; j++)
            {
                chunk[j] = testArray[i * chunkSize + j];
            }
            if (i != 0)
            {
                MPI_Send(chunk, chunkSize, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            else
            {
                // copy into localChunk
                for (int j = 0; j < chunkSize; j++)
                {
                    localChunk[j] = chunk[j];
                }
            }
        }
        // process the local chunk
        int localResult[chunkSize];
        printf("Process %d received chunk of size %d\n", rank, chunkSize);
        time_t runtime = process_chunk(localChunk, chunkSize, localResult);
        printf("Process %d finished in %ld ns\n", rank, runtime);
        // copy localResult into the first chunk of testResult
        for (int i = 0; i < chunkSize; i++)
        {
            testResult[i] = localResult[i];
        }
        // receive the results from the other processes
        for (int i = 1; i < size; i++)
        {
            MPI_Recv(localResult, chunkSize, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // copy localResult into the next chunk of testResult
            for (int j = 0; j < chunkSize; j++)
            {
                testResult[i * chunkSize + j] = localResult[j];
            }
        }
        // end time
        clock_gettime(CLOCK_MONOTONIC, &end);
        // print total runtime
        printf("Total runtime: %ld ns\n", ((end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec)) / 1000);
    }
    else
    {
        // receive the chunk
        int chunkSize = ARRAY_SIZE / size;
        int chunk[chunkSize];
        MPI_Recv(chunk, chunkSize, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received chunk of size %d\n", rank, chunkSize);
        // process the chunk
        int localResult[chunkSize];
        time_t runtime = process_chunk(chunk, chunkSize, localResult);
        printf("Process %d finished in %ld ns\n", rank, runtime);
        // send the result
        MPI_Send(localResult, chunkSize, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}