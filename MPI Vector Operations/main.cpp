#include <iostream>
#include <stdio.h> 
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

using namespace std;

void printArr(int* arr, int n, int start){ // function for displaying array
	for (int i = start; i < n; i++)
	{
		printf(" %4d", arr[i]);
	}
	printf("\n");
}

void randArr(int* arr, int n) { // filling array in

	for (int i = 0; i < n; i++) {

		arr[i] = -20 + rand() % (20 + 20 + 1);
	}
}

void addPart(int* initArr,int start, int* addArr, int n) {

	int p = 0;

	for (int i = start; i < n ; i++) {
		initArr[i] = addArr[p];
		p++;
	}

}

//e = (A + B) * (C + D)
int main(int argc, char* argv[])
{
	srand(time(NULL));

	int rank, size; // processID and amount of processes
	MPI_Status status;
	MPI_Request request;
	int* A, *B, *C, *D;
	int n;
	int nResult = 0;
	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	n = 10 + rand() % (50 - 10 + 1);
	int* resultArrSum1;
	int* resultArrSum2;
	resultArrSum1 = new int[n];
	resultArrSum2 = new int[n];
	// main process
	if (rank == 0) //if proccess id equels 0
	{
		
		
		
		A = new int[n];
		B = new int[n];
		C = new int[n];
		D = new int[n];
		int* finalArr;
		int sendDataSize = 0;
		
		int sumRes = 0;
		randArr(A, n);
		randArr(B, n);
		randArr(C, n);
		randArr(D, n);

		printf("\n");
		printf("process %d, n = %d\n", rank, n);
		printf("starting arrays\n");
		printArr(A, n, 0);
		printArr(B, n, 0);
		printArr(C, n, 0);
		printArr(D, n, 0);
		printf("\n");

		int rest = n % (size - 1);
		int* RestArray1 = new int[rest];
		int* RestArray2 = new int[rest];

		// Sending data start
//////////////////////////////////////////////////////////////////////////////////////////////
		if (rest != 0) // if array members are left 
		{
			sendDataSize = (n - rest) / (size - 1);
			nResult = size;
			printf("rest = %d\n", rest);
			int k = 0;
			for (int i = n - rest; i < n; i++) {
				RestArray1[k] = A[i] + B[i];
				RestArray2[k] = C[i] + D[i];
				k++;
			}
			
			for (int i = 0; i < k; i++) {
				sumRes += RestArray1[i] * RestArray2[i];
			}
		}
		else
		{
			nResult = size - 1;
			sendDataSize = n / (size - 1);
			printf("no rest\n");
		}

		for (int i = 1; i < size; i++)
		{
			int index = (i - 1) * sendDataSize;
			MPI_Send(&A[index], sendDataSize, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&B[index], sendDataSize, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&C[index], sendDataSize, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&D[index], sendDataSize, MPI_INT, i, 1, MPI_COMM_WORLD);
		}


		int ProcSumTmp,ProcSum = 0;
		for (int i = 1; i < size; i++)
		{
			MPI_Recv(&ProcSumTmp, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
			ProcSum += ProcSumTmp;
		}

		ProcSum = ProcSum + sumRes;

		printf("ResultSum=%d", ProcSum);

		//Receiving Data end
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	}

	//slave processes 
	else
	{
		printf("\n process rank %d\n", rank);

		int nRecv = 0;

		MPI_Probe(0, 1, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &nRecv);

		int* recvArr1 = new int[nRecv];
		int* recvArr2 = new int[nRecv];
		int* recvArr3 = new int[nRecv];
		int* recvArr4 = new int[nRecv];
		int* recvArrSum1 = new int[nRecv];
		int* recvArrSum2 = new int[nRecv];

		MPI_Recv(recvArr1, nRecv, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(recvArr2, nRecv, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

		MPI_Recv(recvArr3, nRecv, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(recvArr4, nRecv, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

		for (int i = 0; i < nRecv; i++) {
			recvArrSum1[i] = recvArr1[i] + recvArr2[i];
			recvArrSum2[i] = recvArr3[i] + recvArr4[i];
		}
		int sum = 0;
		for (int i = 0; i < nRecv; i++) {
			sum += recvArrSum1[i] * recvArrSum2[i];
		}

		printf("Sum=%d", sum);
		MPI_Send(&sum, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
	}




	MPI_Finalize();

	return 0;
}

