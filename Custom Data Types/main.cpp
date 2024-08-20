#include <iostream>
#include <time.h>
#include "mpi.h"
using namespace std;

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


const int M = 32;
const int N = 8;





int main(int argc, char** argv) {

    setlocale(LC_ALL, "Russian");
    int ProcNum, ProcRank;

    srand(time(NULL));
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    if (((M - 1) & M) != 0) {
        cout << "The amount of rows doesn't match the task";
    }
    int* elements = NULL;
    int* displ = NULL;

    int* matrix =  new int[N * M];


   
    if (ProcRank == 0) {
        

        for (int i = 0; i < N * M; i++) {
            matrix[i] = rand() % 100;
        }

        for (int i = 0; i < N * M; i++) {
            cout << matrix[i] << " ";
            if (i % N == 0) {
                cout << endl;
            }
        }

    }



    MPI_Datatype oddevenrows;


    MPI_Type_contiguous(N * 2, MPI_INT, &oddevenrows);
    MPI_Type_commit(&oddevenrows),






    elements = new int[ProcNum];
    displ = new int[ProcNum];
    int m = M;
    int tmp_offset = 0;
    for (int i = 0; i < ProcNum; i++) {
        elements[i] = (M * N) / (ProcNum); // elememnts for each process
        // rows
        displ[i] = i * N * 2; // offset
    }




    int n;
    MPI_Scatter(elements, 1, MPI_INT, &n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int* anmatrix = new int[n];
    MPI_Scatterv(matrix, elements, displ, MPI_INT, anmatrix, n, oddevenrows, 0, MPI_COMM_WORLD);

    int max_sum_even = -10000, max_sum_odd = -1000;
    int row_n = n / N;
    for (int i = 0; i < row_n; i++) {
        int sum = 0;
        if ((i % 2) == 0) {

            for (int j = 0; j < N; j++) {
                sum = sum + anmatrix[i * N + j];
            }
            printf("sum: %i\n", sum);
            if (max_sum_even < sum) max_sum_even = sum;
        }
        else {
            for (int j = 0; j < N; j++) {
                sum += anmatrix[i * N + j];
            }
            printf("sum: %i\n", sum);
            if (max_sum_odd < sum) max_sum_odd = sum;
        }

    }
    printf("local max sum even: %i\n", max_sum_even);
    printf("lcoal max sum odd: %i\n", max_sum_odd);

   int max_max_sum_even, max_max_sum_odd;

    MPI_Reduce(&max_sum_even, &max_max_sum_even, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&max_sum_odd, &max_max_sum_odd, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (!ProcRank) {

        printf("\nresult max sum of even rows is %i\n\n", max_max_sum_even);
        printf("\nresult max sum of odd rows is %i\n\n", max_max_sum_odd);
        max_max_sum_even = abs(max_max_sum_even);
        max_max_sum_odd = abs(max_max_sum_odd);

        printf("\nmodule of max sum of even rows is greater than the same module of odd rows in ");
        cout << (float)max_max_sum_even / (float)max_max_sum_odd;


    }

    MPI_Finalize();
    return 0;
