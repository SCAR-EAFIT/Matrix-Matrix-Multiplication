#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cblas.h>
#define N 10

int randomGenerator();

int main(){
    srand((unsigned) time(NULL));
    int size = randomGenerator(), flag = 1, i = 0;
    double timeSpentCheck = 0.0, timeSpentMMM = 0.0;
    double A[size][size] __attribute__((aligned(64))), B[size][size] __attribute__((aligned(64))), C[size][size] __attribute__((aligned(64))), checkMatrix[size][size] __attribute__((aligned(64)));

    while(i++ < N){
        printf("Test %d\n", i);

        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                A[i][j] = randomGenerator(); 
                B[i][j] = randomGenerator(); 
                C[i][j] = 0;
            }
        }

        clock_t beginMMM = clock();
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                for(int k = 0; k < size; k++){
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
        clock_t endMMM = clock();

        clock_t beginCheck = clock();
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, size, size, size, 1.0, &A[0][0], size, &B[0][0], size, 0.0, &checkMatrix[0][0], size);
        clock_t endCheck = clock();

        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                if(checkMatrix[i][j] != C[i][j]){
                    printf("Wrong: checkMatrix[%d]=%lf != C[%d]=%lf\n", i*size + j, checkMatrix[i][j], i*size + j, C[i][j]);
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

    return 0;
}

int randomGenerator(){
    int num = 2 + (rand() % 2000);
    return num;
}