#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Generate an nrows x ncols matrix filled with random values */
int *generate_random_matrix(int nrows, int ncols) {
    int *matrix = (int *)malloc(nrows * ncols * sizeof(int));
    if (matrix == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < nrows * ncols; i++) {
        matrix[i] = rand() % 100;   /* values in [0, 99] */
    }
    return matrix;
}

/* Multiply matrix1 (rows1 x cols1) by matrix2 (rows2 x cols2) into result */
void multiply_matrices(int rows1, int cols1, int *matrix1,
                       int rows2, int cols2, int *matrix2,
                       int *result) {
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            int sum = 0;
            for (int k = 0; k < cols1; k++) {
                sum += matrix1[i * cols1 + k] * matrix2[k * cols2 + j];
            }
            result[i * cols2 + j] = sum;
        }
    }
}

/* Print a matrix to STDOUT */
void display_matrix(int rows, int cols, int *matrix) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

/* Main engine: generate, multiply, time it (once or in a loop).
   Returns the elapsed time (in seconds) of the last multiplication. */
float do_job(int rows1, int cols1, int cols2, int forever) {
    int rows2 = cols1;   /* required for matrix multiplication */
    float elapsed = 0.0f;

    do {
        printf("Generating Matrices...");
        int *A = generate_random_matrix(rows1, cols1);
        printf("Matrix 1 done.\n");
        int *B = generate_random_matrix(rows2, cols2);
        printf("Matrix 2 done.\n");

        int *C = (int *)malloc(rows1 * cols2 * sizeof(int));
        if (C == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            free(A); free(B);
            exit(1);
        }

        clock_t start = clock();
        multiply_matrices(rows1, cols1, A, rows2, cols2, B, C);
        clock_t end = clock();

        elapsed = (float)(end - start) / CLOCKS_PER_SEC;

        time_t now = time(NULL);
        char *timestamp = ctime(&now);
        if (timestamp != NULL) timestamp[24] = '\0';   /* strip newline */

        printf("%s %dx%d matrices: %.6f seconds\n",
               timestamp, rows1, cols1, elapsed);

        free(A);
        free(B);
        free(C);
    } while (forever);

    return elapsed;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <rows1> <cols1> <cols2> <run forever? 0/1>\n",
                argv[0]);
        return 1;
    }

    int rows1   = atoi(argv[1]);
    int cols1   = atoi(argv[2]);
    int cols2   = atoi(argv[3]);
    int forever = atoi(argv[4]);

    srand((unsigned int)time(NULL));

    float t = do_job(rows1, cols1, cols2, forever);
    return 0;
}
