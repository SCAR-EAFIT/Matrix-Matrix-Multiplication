#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cblas.h>
#include <immintrin.h>
#define N 10

int randomGenerator(int op);

int main(){
    srand((unsigned) time(NULL));
    int size = randomGenerator(1), flag = 1, iteration = 0;
    double timeSpentCheck = 0.0, timeSpentMMM = 0.0;
    double *A = (double *)aligned_alloc(32, size * size * sizeof(double));
    double *B = (double *)aligned_alloc(32, size * size * sizeof(double));
    double *C = (double *)aligned_alloc(32, size * size * sizeof(double));
    double *checkMatrix = (double *)aligned_alloc(32, size * size * sizeof(double));
    
    while(iteration++ < N){
        printf("Test %d\n", iteration);
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                int offset = i * size + j;
                A[offset] = randomGenerator(0);
                B[offset] = randomGenerator(0);
                C[offset] = 0;
            }
        }

        clock_t beginMMM = clock();
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                __m256d vec_A = _mm256_setzero_pd();
                vec_A = _mm256_set1_pd(A[i * size + j]);
                for(int k = 0; k < size; k+=4){
                    __m256d vec_multi_res = _mm256_setzero_pd();
                    __m256d vec_B = _mm256_setzero_pd();
                    vec_B = _mm256_load_pd(&B[j * size + k]);
                    vec_multi_res = _mm256_load_pd(&C[i * size + k]);
                    vec_multi_res = _mm256_add_pd(vec_multi_res, _mm256_mul_pd(vec_A, vec_B));
                    _mm256_store_pd(&C[i * size + k], vec_multi_res);
                }
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

    printf("Size of matrix: %d \n", size);
    printf("CBLAS function: %f \n", (timeSpentCheck / N));
    printf("Vectorized function: %f \n", (timeSpentMMM / N));
    printf("Ratio: %f \n", ((timeSpentMMM / N) / (timeSpentCheck / N)));

    free(A);
    free(B);
    free(C);
    free(checkMatrix);

    return 0;
}

int randomGenerator(int op){
    int num = 1;

    if(op == 1){
        while(num % 4 != 0){
            num = 4 + (rand() % 2000);
        }
        return num;
    }

    return num = 4 + (rand() % 2000);
}