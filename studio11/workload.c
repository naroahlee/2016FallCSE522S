 /* This program will create about five seconds of work on each processor
 * on the system.
 *
 * This program requires the OpenMP compiler flag and optimizations turned
 * off to build correctly, e.g.: gcc -fopenmp workload.c -o work
 */

#include <stdio.h> //For printf()
#include <sched.h> //For sched_getcpu()

//ITERS = 500,000,000 will take about five seconds on a 900MHz Raspberry Pi 2
#define ITERS 500000000

void critical_section( void ){
	int index = 0;
	while(index < ITERS){ index++; }
}

int main (int argc, char* argv[]){

	//Create a team of threads on each processor
	#pragma omp parallel
	{
		//Each thread executes this code block independently
		critical_section();
		printf("CPU %d finished!\n", sched_getcpu());
	}

	return 0;
}

