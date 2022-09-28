#include <stdio.h>
#include <string.h>

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
    int matrix[2][3];
    int vector[7];

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

    return 0;
}