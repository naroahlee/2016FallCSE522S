/******************************************************************************
* 
* dense_mm.c
* 
* This program implements a dense matrix multiply and can be used as a
* hypothetical workload. 
*
* Usage: This program takes a single input describing the size of the matrices
*        to multiply. For an input of size N, it computes A*B = C where each
*        of A, B, and C are matrices of size N*N. Matrices A and B are filled
*        with random values. 
*
* Written Sept 6, 2015 by David Ferry
******************************************************************************/

#include <stdio.h>  //For printf()
#include <stdlib.h> //For exit() and atoi()
#include <assert.h> //For assert()
#include <time.h>

const int num_expected_args = 3;
const unsigned sqrt_of_UINT32_MAX = 65536;

// The following line can be used to verify that the parallel computation
// gives identical results to the serial computation. If the verficiation is
// successful then the program executes normally. If the verification fails
// the program will terminate with an assertion error.
//#define VERIFY_PARALLEL

int main( int argc, char* argv[] ){

	int s32cnt;
	int s32i;
	long* ps32record;
	long s32min = 0x7FFFFFFF;
	long s32max = 0;
	double d64total = 0;
	unsigned index, row, col; //loop indicies
	unsigned matrix_size, squared_size;
	double d64avg;
	double *A, *B, *C;
	#ifdef VERIFY_PARALLEL
	double *D;
	#endif
	struct timespec strBefore;
	struct timespec strAfter;

	if( argc != num_expected_args ){
		printf("Usage: ./dense_mm <size of matrices>\n");
		exit(-1);
	}

	matrix_size = atoi(argv[1]);
	s32cnt = atoi(argv[2]);

	ps32record = (long *)malloc(sizeof(long) * s32cnt);
	
	if( matrix_size > sqrt_of_UINT32_MAX ){
		printf("ERROR: Matrix size must be between zero and 65536!\n");
		exit(-1);
	}

	squared_size = matrix_size * matrix_size;

	printf("Generating matrices... \n");

	A = (double*) malloc( sizeof(double) * squared_size );
	B = (double*) malloc( sizeof(double) * squared_size );
	C = (double*) malloc( sizeof(double) * squared_size );
	#ifdef VERIFY_PARALLEL
	D = (double*) malloc( sizeof(double) * squared_size );
	#endif
	for( index = 0; index < squared_size; index++ ){
		A[index] = (double) rand();
		B[index] = (double) rand();
		C[index] = 0.0;
		#ifdef VERIFY_PARALLEL
		D[index] = 0.0;
		#endif
	}
	printf("Multiplying matrices...\n");

	#pragma omp parallel for private(col, row, index)
	for(s32i = 0; s32i < s32cnt; s32i++)
	{
		(void)clock_gettime(CLOCK_MONOTONIC_RAW, &strBefore);
		for( col = 0; col < matrix_size; col++ ){
			for( row = 0; row < matrix_size; row++ ){
				for( index = 0; index < matrix_size; index++){
				C[row*matrix_size + col] += A[row*matrix_size + index] *B[index*matrix_size + col];
				}	
			}
		}

		(void)clock_gettime(CLOCK_MONOTONIC_RAW, &strAfter);
		ps32record[s32i] = (long)((strAfter.tv_sec - strBefore.tv_sec) * 10e9L) 
						   + (strAfter.tv_nsec - strBefore.tv_nsec);
		printf("%ld\n", ps32record[s32i]);
	}
	
	#ifdef VERIFY_PARALLEL
	printf("Verifying parallel matrix multiplication...\n");
	for( col = 0; col < matrix_size; col++ ){
		for( row = 0; row < matrix_size; row++ ){
			for( index = 0; index < matrix_size; index++){
			D[row*matrix_size + col] += A[row*matrix_size + index] *B[index*matrix_size + col];
			}	
		}
	}

	for( index = 0; index < squared_size; index++ ) 
		assert( C[index] == D[index] );
	#endif //ifdef VERIFY_PARALLEL

	printf("Multiplication done!\n");

	/* Report */
	d64total = 0.0;
	for(s32i = 0; s32i < s32cnt; s32i++)
	{
		d64total += ps32record[s32i];
		if(ps32record[s32i] >= s32max)
		{
			s32max = ps32record[s32i];
		}
		if(ps32record[s32i] <= s32min)
		{
			s32min = ps32record[s32i];
		}
	}	
	d64avg = d64total / s32cnt;

	printf("Max %ld ns\nMin %ld ns\nAvg %lf ns\n",
			s32max, 
			s32min, 
			d64avg);
	

	free(ps32record);
	return 0;
}
