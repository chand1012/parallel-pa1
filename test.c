#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 10

int randint(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

int main(void)
{
    srand(time(NULL));
    int m, n;

    // prompt the user for the matrix dimensions
    printf("Enter the minimum number: ");
    scanf("%d", &m);

    printf("Enter the maximum number: ");
    scanf("%d", &n);

    int testArray[ARRAY_SIZE];

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        testArray[i] = randint(m, n);
    }

        return 0;
}