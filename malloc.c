#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cblas.h>
#include <immintrin.h>
#define N 10

int randomGenerator();

int main(){
    srand((unsigned) time(NULL));
    int size = randomGenerator(), flag = 1, iteration = 0;
    double timeSpentCheck = 0.0, timeSpentMMM = 0.0;
    double *A = (double *)aligned_alloc(256, size * size * sizeof(double)); //double = 64 bits
    double *B = (double *)aligned_alloc(256, size * size * sizeof(double));
    double *C = (double *)aligned_alloc(256, size * size * sizeof(double));
    double *checkMatrix = (double *)aligned_alloc(256, size * size * sizeof(double));
    
    while(iteration++ < N){
        printf("Test %d\n", iteration);
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                int offset = i * size + j;
                A[offset] = randomGenerator();
                B[offset] = randomGenerator();
                C[offset] = 0;
            }
        }

        clock_t beginMMM = clock();
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                __m256d c_values = _mm256_setzero_pd();
                for(int k = 0; k < size; k+=4){
                    __m256d a_data = _mm256_load_pd(&A[i * size + k]);
                    __m256d b_data = _mm256_load_pd(&B[k * size + j]);
                    c_values = _mm256_fmadd_pd(a_data, b_data, c_values);
                }
                double result[4];
                _mm256_store_pd(result, c_values);
                C[i * size + j] += result[0] + result[1] + result[2] + result[3];
            }
        }

        clock_t endMMM = clock();

        clock_t beginCheck = clock();
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, size, size, size, 1.0, A, size, B, size, 0.0, checkMatrix, size);
        clock_t endCheck = clock();

        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                if(checkMatrix[i * size + j] != C[i * size + j]){
                    printf("Wrong: checkMatrix[%d]=%lf != C[%d]=%lf\n", i*size + j, checkMatrix[i * size + j], i*size + j, C[i * size + j]);
                    flag = 0;
                    break;
                }
            }
            if(flag == 0){
                break;
            }
        }
        
        timeSpentCheck += (double)(endCheck - beginCheck) / CLOCKS_PER_SEC;
        timeSpentMMM += (double)(endMMM - beginMMM) / CLOCKS_PER_SEC;
    }

    if(flag == 1){
        printf("Size of matrices: %d \n", size);
        printf("CBLAS function: %f \n", (timeSpentCheck / N));
        printf("Our function: %f \n", (timeSpentMMM / N));
        printf("Ratio: %f \n", ((timeSpentMMM / N) / (timeSpentCheck / N)));
    }

    free(A);
    free(B);
    free(C);
    free(checkMatrix);

    return 0;
}

int randomGenerator(){
    int num = 1;
    while(num % 4 != 0){
        num = 4 + (rand() % 2000);
    }
    return num;
}