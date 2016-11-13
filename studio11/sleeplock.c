// Sleeplock.c studio11
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
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/time.h>
#include <limits.h>

//ITERS = 500,000,000 will take about five seconds on a 900MHz Raspberry Pi 2
#define ITERS 500000000

#define UNLOCKED (1)
#define LOCKED   (0)

volatile int gs32curlock = UNLOCKED;

void lock(volatile int* ps32lock)
{
	int s32ret;
	while(1)
	{
		s32ret = __atomic_sub_fetch(ps32lock, 1, __ATOMIC_ACQ_REL);
		if(s32ret < 0)
		{
			__atomic_store_n(ps32lock, -1, __ATOMIC_RELEASE);
			syscall(SYS_futex, ps32lock, FUTEX_WAIT, -1, NULL);
			/* Attention: When we are waking up some time,  */
			/* we will continue trying to fetch the lock,   */
			/* rather than exit */
		}
		else
		{
			/* only if we successfully get the lock, exit */ 
			break;
		}
	}
	return;
}

void unlock(volatile int* ps32lock)
{
	/* Attention: Unlock() would never be invoked simutanously  */
	/* And for a binaray semephone, we must set it to UNLOCK(1) */
	int s32ret;
	s32ret = __atomic_add_fetch(ps32lock, 1, __ATOMIC_ACQ_REL);
	if(1 != s32ret)
	{
		/* Only if some other sleeping processes exist */
		/* Say, we have *ps32lock == -1 before enter   */
		/* this unlock() fxn.						   */
		__atomic_store_n(ps32lock, 1, __ATOMIC_RELEASE);
		syscall(SYS_futex, ps32lock, FUTEX_WAKE, INT_MAX);
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
		critical_section();
		//printf("CPU %d out critical! lock = %d\n", sched_getcpu(), gs32curlock);
		unlock(&gs32curlock);
		printf("CPU %d finished!\n", sched_getcpu());
	}

	return 0;
}

