#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//http://www.sciencedirect.com/science/article/pii/S0019995873902283/pdf?md5=37b2d0aafda0b0639e48370c73d5e82a&pid=1-s2.0-S0019995873902283-main.pdf
//https://pl.wikipedia.org/wiki/Macierz_logiczna

// Used later for threads
struct arguments {
    int **firstmatrix;
    int **secondmatrix;
    int **finalmatrix;
    int **transposedmatrix;
    int start;
    int step;
    int size;
};

void showMatrix(int **matrix, int size);
int **createRandomMatrix(int size);
int **transpose_matrix(int **matrix, int size, int **transposed);
void loopnest(int **firstmatrix, int **transposedmatrix, int size, int **finalmatrix);

// Very similiar to multiplication of a matrixs, basically adding
// access to the structure "arguments" via "a" pointer
void *thread_function_normal(void *args) {
    struct arguments *a = args;
    int m = 0;

    for (int x = 0; x < a->size; x++) {
        for (int y = 0; y < a->size; y++) {
            if(m == a->start) {
                a->finalmatrix[x][y] = 0;
                for (int z = 0; z < a->size; z++) {
                    a->finalmatrix[x][y] += a->firstmatrix[x][z] * a->secondmatrix[z][y];
                }
                a->start+=a->step;
            }
            m++;
        }
    }

    free(args);
    return NULL;
}

// We need no longer z to go trough columns and rows at one time
void *thread_function_transposed(void *args) {
    struct arguments *a = args;
    int m = 0;
    for (int x = 0; x < a->size; x++) {
        for (int y = 0; y < a->size; y++) {
            if(m == a->start) {
                a->finalmatrix[x][y] = 0;
                for (int z = 0; z < a->size; z++) {
                    a->finalmatrix[x][y] += a->firstmatrix[x][z] * a->transposedmatrix[y][z];
                }
                a->start+=a->step;
            }
            m++;
        }
    }

    free(args);
    return NULL;
}

void *thread_function_loopnest(void *args) {
    int acc00, acc01, acc10, acc11;
    struct arguments *a = args;


  for (int i = 0; i < a->size; i += 2) {
    for (int j = 0; j < a->size; j += 2) {
      acc00 = acc01 = acc10 = acc11 = 0;
      for (int k = 0; k < a->size; k++) {
        acc00 += a->firstmatrix[j + 0][k] * a->transposedmatrix[i + 0][k];
        acc01 += a->firstmatrix[j + 1][k] * a->transposedmatrix[i + 0][k];
        acc10 += a->firstmatrix[j + 0][k] * a->transposedmatrix[i + 1][k];
        acc11 += a->firstmatrix[j + 1][k] * a->transposedmatrix[i + 1][k];
      }
      a->finalmatrix[i + 0][j + 0] = acc00;
      a->finalmatrix[i + 0][j + 1] = acc01;
      a->finalmatrix[i + 1][j + 0] = acc10;
      a->finalmatrix[i + 1][j + 1] = acc11;
    }
  }
}

int main(void) {
    // Seed for generating random integers from computer internal clock
    srand(time(NULL));

    int size;
    int threads_num;
    int **firstmatrix;
    int **secondmatrix;
    int **transposedmatrix;
    int **finalmatrix;
    int **finalmatrix2;

    // We are asking for the input
    // Matrix will be size x size
    printf("|-|-| Type in Size of Matrix and # of threads:\n|-| (Size / #Threads): ");
    scanf("%d %d", &size, &threads_num);

    // If we more threads than there is fields in the matrix
    // Then we change it to the x*y of that matrix
    if(threads_num > size * size) {
        threads_num = size * size;
    }

    // We are setting matrixes and preparing them to be filled in
    firstmatrix = createRandomMatrix(size);
    secondmatrix = createRandomMatrix(size);
    // This is full of gibberish right now, but one line below we will use threads to fill it with proper values
    finalmatrix = createRandomMatrix(size);
    finalmatrix2 = createRandomMatrix(size);
    // As above
    transposedmatrix = createRandomMatrix(size);




    // ==================== Here we go with first calculation w/o transposition: ====================
    // Clock to count time
    // About gettimeofday - https://blog.habets.se/2010/09/gettimeofday-should-never-be-used-to-measure-time.html
    clock_t start = clock();

    // Threads
    // https://pl.wikibooks.org/wiki/POSIX_Threads/Podstawowe_operacje

    pthread_t threads[threads_num];

    for (int i = 0; i < threads_num; i++) {
        // Structure of args for a thread
        struct arguments *args = malloc(sizeof(struct arguments));
        args->firstmatrix = firstmatrix;
        args->secondmatrix = secondmatrix;
        args->finalmatrix = finalmatrix;
        args->start = i;
        args->step = threads_num;
        args->size = size;

        //C reate the thread
        pthread_create(&threads[i], NULL, &thread_function_normal, (void *) args);
    }

    // Ignore the result, just wait for the threads
    for (int i = 0; i < threads_num; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end = clock();
    double time = (end - start)/(double)CLOCKS_PER_SEC;
    printf("|-| It took %lf to calculate normally.\n", time);
    /*
    printf("___\n#first1:\n");
    showMatrix(firstmatrix, size);

    printf("___\n#second1:\n");
    showMatrix(secondmatrix, size);

    printf("___\n#final1:\n");
    showMatrix(finalmatrix, size);*/

    // ==================== Transposing Second Matrix ====================
    transpose_matrix(secondmatrix, size, transposedmatrix);
    /* Debug to check if correct
    printf("___\n#second:\n");
    showMatrix(secondmatrix, size);
    printf("___\n#trans:\n");
    showMatrix(transposedmatrix, size);*/




    clock_t start2 = clock();

    // Threads
    // https://pl.wikibooks.org/wiki/POSIX_Threads/Podstawowe_operacje

    pthread_t threads2[threads_num];

    for (int i = 0; i < threads_num; i++) {
        struct arguments *args = malloc(sizeof(struct arguments));
        args->firstmatrix = firstmatrix;
        args->transposedmatrix = transposedmatrix;
        args->finalmatrix = finalmatrix;
        args->start = i;
        args->step = threads_num;
        args->size = size;
        pthread_create(&threads[i], NULL, &thread_function_transposed, (void *) args);
    }

    // Ignore the result, just wait for the threads
    for (int i = 0; i < threads_num; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end2 = clock();
    double time2 = (end2 - start2)/(double)CLOCKS_PER_SEC;
    printf("|-| It took %lf to calculate with transposition.\n", time2);

    /*printf("___\n#first1:\n");
    showMatrix(firstmatrix, size);

    printf("___\n#transposed:\n");
    showMatrix(transposedmatrix, size);

    printf("___\n#final2:\n");
    showMatrix(finalmatrix, size);*/

    



    clock_t start3 = clock();
    /*
    // Threads
    // https://pl.wikibooks.org/wiki/POSIX_Threads/Podstawowe_operacje

    pthread_t threads3[threads_num];

    for (int i = 0; i < threads_num; i++) {
        struct arguments *args = malloc(sizeof(struct arguments));
        args->firstmatrix = firstmatrix;
        args->transposedmatrix = transposedmatrix;
        args->finalmatrix = finalmatrix;
        args->start = i;
        args->step = threads_num;
        args->size = size;
        pthread_create(&threads[i], NULL, &thread_function_loopnest, (void *) args);
    }

    // Ignore the result, just wait for the threads
    for (int i = 0; i < threads_num; i++) {
        pthread_join(threads[i], NULL);
    }
    */
   int i = 0;
   int j = 0;
   int k = 0;
   int acc00, acc01, acc10, acc11 = 0;;
    for (i = 0; i < size; i += 2)
    {
        for (j = 0; j < size; j += 2)
        {
            acc00 = acc01 = acc10 = acc11 = 0;
            for (k = 0; k < size; k++)
            {
                acc00 += firstmatrix[j + 0][k] * transposedmatrix[i + 0][k];
                acc01 += firstmatrix[j + 1][k] * transposedmatrix[i + 0][k];
                acc10 += firstmatrix[j + 0][k] * transposedmatrix[i + 1][k];
                acc11 += firstmatrix[j + 1][k] * transposedmatrix[i + 1][k];
            }
            finalmatrix2[i + 0][j + 0] = acc00;
            finalmatrix2[i + 1][j + 0] = acc01;
            finalmatrix2[i + 0][j + 1] = acc10;
            finalmatrix2[i + 1][j + 1] = acc11;
        }
    }


    clock_t end3 = clock();
    double time3 = (end3 - start3)/(double)CLOCKS_PER_SEC;
    printf("|-| It took %lf to calculate with transposition and loop nest.\n", time3);

    /*printf("___\n#first1:\n");
    showMatrix(firstmatrix, size);

    printf("___\n#transposed:\n");
    showMatrix(transposedmatrix, size);

    printf("___\n#final2:\n");
    showMatrix(finalmatrix2, size);*/






    return 0;
}

int **createRandomMatrix(int size) {
    // Points to beginning of the matrix
    int **matrix = (int **) malloc(size * sizeof(int *));

    // Simple double for for creating a square of size [size, size]
    for (int i = 0; i < size; i++) {
        int *row = malloc(size * sizeof(int));
        matrix[i] = row;
        for (int j = 0; j < size; j++) {
            // Modulo 10 will make the matrix in decimals
            matrix[i][j] = (rand() % 10);
        }
    }
    return matrix;
}

void showMatrix(int **matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
}
//Cause it's using threads then we need to compile with "gcc -pthread zad2.c"

int **transpose_matrix(int **matrix, int size, int **transposed) {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            transposed[y][x] = matrix[x][y];
        }
    }
}

    // Printing the result in console
    /*printf("___\n#1:\n");
    showMatrix(firstmatrix, size);

    printf("___\n#2:\n");
    showMatrix(secondmatrix, size);

    printf("________\n#1 * #2:\n");
    showMatrix(finalmatrix, size);*/

