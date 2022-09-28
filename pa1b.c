#include <stdio.h>
#include <string.h>
#include <mpi.h>

void load_file(const char *fname, int matrix[2][3], int vector[7])
{
    int temp_matrix[7][3];
    // load the file
    FILE *fp = fopen(fname, "r");
    char line[1000] = "";

    ssize_t row = 0;

    // read the file line by line
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        int x = -1;
        int y = -1;
        int z = -1;
        sscanf(line, "%d %d %d", &x, &y, &z);

        vector[row] = x;

        temp_matrix[row][0] = x;
        temp_matrix[row][1] = y;
        temp_matrix[row][2] = z;

        // clear the line buffer
        memset(line, 0, sizeof(line));
        ++row;
    }

    // load the only two rows we need into the matrix
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            matrix[i][j] = temp_matrix[i + 2][j];
        }
    }
}

int main(void)
{
    const unsigned int sz = 128;
    int vector[7];
    int matrix[2][3];
    char buffer[sz];
    char line[1000] = "";

    int size, rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        load_file("test_files/mv-data.txt", matrix, vector);

        // print the matrix
        printf("Input Matrix:\n");
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                printf("%d ", matrix[i][j]);
            }
            printf("\n");
        }

        // print the vector
        printf("Input Vector:\n");
        for (int i = 0; i < 7; i++)
        {
            printf("%d ", vector[i]);
        }
        printf("\n");

        // chunk the matrix into two parts
        int matrix_chunk_a[3] = {matrix[0][0], matrix[0][1], matrix[0][2]};
        int matrix_chunk_b[3] = {matrix[1][0], matrix[1][1], matrix[1][2]};

        // send matrix chunk b to process 1
        MPI_Send(matrix_chunk_b, 3, MPI_INT, 1, 0, MPI_COMM_WORLD);
        // send the vector to process 1
        MPI_Send(vector, 7, MPI_INT, 1, 0, MPI_COMM_WORLD);

        // multiply the matrix chunk a by the vector
        int result_a[7] = {0, 0, 0, 0, 0, 0, 0};
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 7; j++)
            {
                result_a[j] += matrix_chunk_a[i] * vector[j];
            }
        }

        // receive the result from process 1
        int result_b[7];
        MPI_Recv(result_b, 7, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // print the results
        printf("Result:\n");
        for (int i = 0; i < 7; i++)
        {
            printf("%i ", result_a[i] + result_b[i]);
        }
        printf("\n");
    }
    else
    {
        int matrix_chunk_b[3];
        int vector[7];

        // receive the matrix chunk b from process 0
        MPI_Recv(matrix_chunk_b, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // receive the vector from process 0
        MPI_Recv(vector, 7, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // multiply the matrix chunk b by the vector
        int result_b[7] = {0, 0, 0, 0, 0, 0, 0};
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 7; j++)
            {
                result_b[j] += matrix_chunk_b[i] * vector[j];
            }
        }

        // send the result to process 0
        MPI_Send(result_b, 7, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
