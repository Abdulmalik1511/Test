#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
// jffskljlfldkksdlkd
int *A;                  // Shksdamamsals,l,ared array A tesetttt
int *C;                  // Shared array C
int **B;                 // Shared array B of pointers
int N, T;                // Number of integers and number of threads
int next_digit = 0;      // Shared variable for synchronization
pthread_mutex_t lock;    // Mutex lock for synchronization

void *sort_subarray(void *arg) {
    while (1) {
        int i;

        // Safely increment next_digit
        pthread_mutex_lock(&lock);
        i = next_digit++;
        pthread_mutex_unlock(&lock);

        if (i >= 10) break; // Exit if all digits are processed

        if (C[i] > 0) {
            // Allocate memory for B[i]
            B[i] = (int *)malloc(C[i] * sizeof(int));
            if (!B[i]) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }

            // Copy elements from A to B[i]
            int idx = 0;
            for (int j = 0; j < N; j++) {
                if (A[j] / 100 == i) {
                    B[i][idx++] = A[j];
                }
            }

            // Sort B[i]
            for (int x = 0; x < C[i] - 1; x++) {
                for (int y = x + 1; y < C[i]; y++) {
                    if (B[i][x] > B[i][y]) {
                        int temp = B[i][x];
                        B[i][x] = B[i][y];
                        B[i][y] = temp;
                    }
                }
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <N> <T>\n", argv[0]);
        return EXIT_FAILURE;
    }

    N = atoi(argv[1]);
    T = atoi(argv[2]);

    if (N <= 0 || T <= 0) {
        fprintf(stderr, "N and T must be positive integers.\n");
        return EXIT_FAILURE;
    }

    // Initialize shared arrays
    A = (int *)malloc(N * sizeof(int));
    C = (int *)calloc(10, sizeof(int));
    B = (int **)calloc(10, sizeof(int *));
    if (!A || !C || !B) {
        perror("Memory allocation failed");
        return EXIT_FAILURE;
    }

    pthread_mutex_init(&lock, NULL);

    // Generate random numbers and populate A and C
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        A[i] = rand() % 1000; // Random number in range 000-999
        C[A[i] / 100]++;
    }

    // Create threads
    pthread_t threads[T];
    for (int i = 0; i < T; i++) {
        if (pthread_create(&threads[i], NULL, sort_subarray, NULL) != 0) {
            perror("Thread creation failed");
            return EXIT_FAILURE;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
    }

    // Merge sorted subarrays back into A
    int idx = 0;
    for (int i = 0; i < 10; i++) {
        if (C[i] > 0) {
            for (int j = 0; j < C[i]; j++) {
                A[idx++] = B[i][j];
            }
            free(B[i]); // Free memory allocated for B[i]
        }
    }

    // Output sorted array A
    printf("Sorted array A:\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", A[i]);
    }
    printf("\n");

    // Clean up
    free(A);
    free(C);
    free(B);
    pthread_mutex_destroy(&lock);

    return 0;
}
