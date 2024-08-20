
#include <iostream>
#include <time.h>
#include <omp.h>
using namespace std;

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define M  32
#define N  8


int main(int argc, char** argv) {

    int ProcNum = 4, ProcRank;
    int sum = 0;
    int q = 4;
    int mtrI[M * N];
    int* mtrQ =  new int[8 * 8];
    double* mtrR = new double[M * N];
    double start, end;


    for (int i = 0; i < N * M; i++) {
        mtrI[i] = -128 + rand() % (127 - (-128));
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            mtrQ[i * 8 + j] = 1 + ((1 + i + j) * q);
        }
        
    }
    
    cout << "Matrix I" << endl;

    for (int i = 0; i < M * N; i++) {
        
        if (i % 8 == 0) {
            cout << endl;
        }
        cout << mtrI[i]<<"\t";

    }


    cout << "\n" << "Matrix Q" << endl;

    for (int i = 0; i < 8 * 8; i++) {

        if (i % 8 == 0) {
            cout << endl;
        }
        cout << mtrQ[i] << "\t";

    }


    start = omp_get_wtime();
    omp_set_num_threads(ProcNum);
    int i, j, k,p=0;
#pragma omp parallel for shared(mtrI, mtrQ, mtrR) private(i,j,k)
    for (i = 0; i < M; i += 8) {
           
            for (k = 0; k < 8 * 8; k++) {
                mtrR[i * N + k] = 0;
                mtrR[i * N + k] = (double)(((double)mtrI[i * N + k] / (double)mtrQ[k]));
            }
    }
    


    end = omp_get_wtime();

    cout <<"\n"<<"Matrix R"<<endl;


    for (int i = 0; i < M * N; i++) {

        if (i % 8 == 0) {
            cout << endl;
        }
        printf("%7.3f", mtrR[i]);

    }


    return 0;
}
