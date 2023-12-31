#include <iostream>
#include <ctime>

using namespace std;

int randomGenerator();

int main(){
    srand((unsigned) time(NULL));
    int size = randomGenerator();
    cout << size << endl;
    int A[size][size], B[size][size], C[size][size];

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            int n1 = randomGenerator();
            int n2 = randomGenerator();
            A[i][j] = n1; 
            B[i][j] = n2; 
            C[i][j] = 0;
        }
    }

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            for(int k = 0; k < size; k++){
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            cout.width(10);
            cout << C[i][j];
        }
        cout << endl;
    }

    return 0;
}

int randomGenerator(){
    int num = 2 + (rand() % 9);
    return num;
}