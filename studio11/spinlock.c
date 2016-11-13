// spinlock.c Studio 11
/*    Simulated workload with OpenMP
 *
 * This program will create about five seconds of work on each processor
 * on the system.
 *
 * This program requires the OpenMP compiler flag and optimizations turned
 * off to build correctly, e.g.: gcc -fopenmp workload.c -o work
 */

#include <stdio.h> //For printf()
#include <stdlib.h>
#include <sched.h> //For sched_getcpu()

//ITERS = 500,000,000 will take about five seconds on a 900MHz Raspberry Pi 2
#define ITERS 500000000

#define LOCK   (1)
#define UNLOCK (0)

volatile int gs32curlock = UNLOCK;

void lock(volatile int* ps32lock)
{
	/* These two var must be local */
	int s32lock    = LOCK;
	int s32unlock  = UNLOCK;

	while(0 ==  __atomic_compare_exchange(ps32lock, &s32unlock, &s32lock, 1, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))
	{
		/* if we fail, that means gs32curlock == LOCK and gs32unlock is going to assigned as LOCK */
		/* And we will continue trying */
		/* We need to reset the unlock */
		s32unlock = UNLOCK;
	}
	/* if we success, that means gs32curlock == UNLOCK and finally was assigned as LOCK */ 
	return;
}

void unlock(volatile int* ps32lock)
{
	int s32ret;
	/* These two var must be local */
	int s32lock    = LOCK;
	int s32unlock  = UNLOCK;
	s32ret = __atomic_compare_exchange(ps32lock, &s32lock, &s32unlock, 1, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
	if(0 == s32ret)
	{
		fprintf(stderr, "Error! CPU %d Cannot unlock\n", sched_getcpu());
		exit (-1);
	}
	return;
}

void critical_section(void)
{
	int index = 0;
	while(index < ITERS)
	{
		index++;
	}
	return;
}

int main (int argc, char* argv[])
{
	//Create a team of threads on each processor
	#pragma omp parallel
	{
		//Each thread executes this code block independently
		lock(&gs32curlock);
		//printf("CPU %d is now in Critical Section!\n", sched_getcpu());
		critical_section();
		unlock(&gs32curlock);
		printf("CPU %d finished!\n", sched_getcpu());
	}

	return 0;
}

