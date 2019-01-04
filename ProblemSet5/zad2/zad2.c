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
    int start;
    int step;
    int size;
};

void showMatrix(int **matrix, int size);
int **createRandomMatrix(int size);

// Very similiar to creation of a matrix, basically adding
// access to the structure "arguments" via "a" pointer
void *thread_function(void *args) {
    struct arguments *a = args;
    int m = 0;

    for (int x = 0; x < a->size; x++) {
        for (int y = 0; y < a->size; y++) {
            if(m == a->start) {
                a->finalmatrix[x][y] = 0;
                for (int z = 0; z < a->size; z++) {
                    // if value is above 1, then we dont care about that in boolean matrix, its just true
                    a->finalmatrix[x][y] = (a->firstmatrix[x][z] * a->secondmatrix[z][y] > 0);
                }
                a->start+=a->step;
            }
            m++;
        }
    }

    free(args);
    return NULL;
}

int main(void) {
    // Seed for generating random integers from computer internal clock
    srand(time(NULL));

    int size;
    int threads_num;
    int **firstmatrix;
    int **secondmatrix;
    int **finalmatrix;

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
        pthread_create(&threads[i], NULL, &thread_function, (void *) args);
    }

    // Ignore the result, just wait for the threads
    for (int i = 0; i < threads_num; i++) {
        pthread_join(threads[i], NULL);
    }

    // Printing the result in console
    printf("___\n#1:\n");
    showMatrix(firstmatrix, size);

    printf("___\n#2:\n");
    showMatrix(secondmatrix, size);

    printf("________\n#1 * #2:\n");
    showMatrix(finalmatrix, size);

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
            // Modulo 2 will make the matrix full of 0's or 1's
            matrix[i][j] = (rand() % 2);
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