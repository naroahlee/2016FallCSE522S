#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include "profiling.h"
#include "parsing.h"


/* Shared Global Variable */
prof_arg gstrArg = 
{
	.s32pthpercore = 2,
	.as32prio = {0, 0},
	.s32sched_policy = SCHED_OTHER,
	.s32round  = 0,
	.s32iter   = 0,
	.s32cnt    = 0,
	.ps32cabin = NULL,
};

/* I need a barrier */
#ifdef CUSTOM_SLEEP_BARRIER
pthread_mutex_t gmutex_barrier = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gcond_var;
int gs32remain = 0;

#elif CUSTOM_SPIN_BARRIER

int gs32MAX_THREAD = 0;
int gs32bar = 0;             /* Counter of threads, faced barrier. */
volatile int gs32passed = 0; /* Number of barriers, passed by all threads. */

void barrier_wait()
{
    int s32passed_old = gs32passed; /* Should be evaluated before incrementing s32bar! */

    if(__sync_fetch_and_add(&gs32bar, 1) == (gs32MAX_THREAD - 1))
    {
        // The last thread, faced barrier.
        gs32bar = 0;
        // *bar* should be reseted strictly before updating of barriers counter.
        __sync_synchronize(); 
        gs32passed++; // Mark barrier as passed.
    }
    else
    {
        // Not the last thread. Wait others.
        while(gs32passed == s32passed_old)
		{
		};
        // Need to synchronize cache with other threads, passed barrier.
        __sync_synchronize();
    }
}

#else
pthread_barrier_t gstrbar;
#endif

/* Global idx which is responsible for pointer the cabin list */
int gs32cabinidx = 0;
#ifdef USE_SPINLOCK
pthread_spinlock_t gspin_cabinidx;
#else
/* Default: Use Mutex */
pthread_mutex_t gmutex_cabinidx = PTHREAD_MUTEX_INITIALIZER;
#endif

void set_pthread_schedinfo_attr(pthread_attr_t * pstrattr,
			                    int s32policy,
								int s32prio,
								int s32coreid)
{

    struct sched_param strsched;
#ifndef DISABLE_PIN_CORE
    cpu_set_t          strmask;
#endif

	/* Init Attr */
	pthread_attr_init(pstrattr);

	/* Pin to a dedicated Core */
#ifndef DISABLE_PIN_CORE
    CPU_ZERO(&strmask);
    CPU_SET(s32coreid, &strmask);
    if(0 != pthread_attr_setaffinity_np(pstrattr,
						                sizeof(cpu_set_t),
                                        &strmask))
	{
		print_error(ERR_PIN_CORE);
		exit(ERR_PIN_CORE);
	}
#endif

	/* Set Sched and Priority */
	switch(s32policy)
	{
	case SCHED_OTHER:
		strsched.sched_priority = 0;
		break;
	case SCHED_RR:
	case SCHED_FIFO:
		strsched.sched_priority = s32prio;
		break;
	default:
		print_error(ERR_SET_POLICY);
		exit(ERR_SET_POLICY);
	}

	/* We should set policy first */
	if(0 != pthread_attr_setschedpolicy(pstrattr, s32policy))
	{
		print_error(ERR_SET_POLICY);
		exit(ERR_SET_POLICY);
	}
	if(0 != pthread_attr_setschedparam (pstrattr, &strsched))
	{
		print_error(ERR_SET_PRIORITY);
		exit(ERR_SET_PRIORITY);
	}

	pthread_attr_setinheritsched(pstrattr, PTHREAD_EXPLICIT_SCHED);

	return;
}


void* work_fxn(void* parg)
{
	int s32tid;
	int s32prio;
	int s32round;
	int s32roundmax;
	int s32iter;
	int s32itermax;
	int s32cnt;
	int* ps32cabin;

	int s32idx;
	volatile int s32base;
	volatile int s32cuberes __attribute__((unused));

	/* Get Parameter */
	work_arg* pstrworkarg;
	prof_arg* pstrprofarg;
	pstrworkarg = (work_arg*)parg;
	pstrprofarg = pstrworkarg->pstrprofarg;
	
	s32prio     = pstrworkarg->s32prio;
	s32roundmax = pstrprofarg->s32round;
	s32itermax  = pstrprofarg->s32iter;
	s32cnt      = pstrprofarg->s32cnt;
	ps32cabin   = pstrprofarg->ps32cabin;

	if(pstrprofarg->s32sched_policy == SCHED_OTHER)
	{
		s32tid = syscall(SYS_gettid);
		setpriority(PRIO_PROCESS, s32tid, s32prio);
	}
	/* ps32cabin referred to a chunk of global shared memory, */
	/* while other fields can be local */
	
	/* Wait for barrier */
#ifdef CUSTOM_BARRIER
	pthread_mutex_lock(&gmutex_barrier);
	gs32remain--;
	if(0 == gs32remain)
	{
		pthread_cond_broadcast(&gcond_var);
	}
	else
	{
		while(0 != gs32remain)
		{
			pthread_cond_wait(&gcond_var, &gmutex_barrier);
		}
	}
	pthread_mutex_unlock(&gmutex_barrier);

#elif CUSTOM_SPIN_BARRIER
	barrier_wait();
#else
	pthread_barrier_wait(&gstrbar);
#endif

	for(s32round = 0; s32round < s32roundmax; s32round++)
	{
		/* At the start of every round */
		/* Pick up a new number from cabin list */
#ifdef USE_SPINLOCK
		pthread_spin_lock(&gspin_cabinidx);
#else
		pthread_mutex_lock(&gmutex_cabinidx);
#endif
		s32idx = gs32cabinidx;
		gs32cabinidx = (gs32cabinidx + 1) % s32cnt;
#ifdef USE_SPINLOCK
		pthread_spin_unlock(&gspin_cabinidx);
#else
		pthread_mutex_unlock(&gmutex_cabinidx);
#endif

		s32base = ps32cabin[s32idx];
		/* Start Iter for Every round */
		/* Simulated workload */
		for(s32iter = 0; s32iter < s32itermax; s32iter++)
		{
			/* s32cuberes = s32base * s32base * s32base; */
			/* Using a much more time-consuming fxn */
			s32cuberes = (int)pow((double)s32base, 3.0);
		}
	}

	return NULL;
}


void useage(void)
{
	printf("Useage:\n\t./run <Sched> <Round> <Iter> <Data0> [Data1 ...]\n");
	return;
}

void useage_cfg(void)
{
	printf("ext.cfg should be\n\t<Num of Threads per core> <Priority1> <Priority2>\n");
	return;
}


int main(int argc, char* argv[])
{
	int s32ret;
	int s32i;
	int s32j;
	int s32idx;
	int s32prio;
	long s32PCPUcnt;
	int s32threadcnt;
	work_arg* pstrworkarg;
	pthread_t* ppthread;
	pthread_attr_t* pstrattr;

	/* Parsing Input and Robustness Check */
	s32ret = parsing_input(argc, argv, &gstrArg);
	if(0 != s32ret)
	{
		print_error(s32ret);
		useage();
		exit(s32ret);
	}
	s32ret = parsing_cfgfile(&gstrArg);
	if(0 != s32ret)
	{
		print_error(s32ret);
		useage_cfg();
		exit(s32ret);
	}

#if DBG_LVL >= 2
	print_prof_arg(&gstrArg);
#endif

	/* get PCPU number */
	s32PCPUcnt   = (int)sysconf(_SC_NPROCESSORS_ONLN);
	s32threadcnt = (int)s32PCPUcnt * gstrArg.s32pthpercore;
	ppthread    = (pthread_t*)     malloc(s32threadcnt * sizeof(pthread_t));
	pstrattr    = (pthread_attr_t*)malloc(s32threadcnt * sizeof(pthread_attr_t));
	pstrworkarg = (work_arg*)      malloc(s32threadcnt * sizeof(work_arg));
#ifdef USE_SPINLOCK
	if(0 != pthread_spin_init(&gspin_cabinidx, PTHREAD_PROCESS_PRIVATE))
	{
		print_error(ERR_INIT_MUTEX);
		exit(ERR_INIT_MUTEX);
	}
#else
	if(0 != pthread_mutex_init(&gmutex_cabinidx, NULL))
	{
		print_error(ERR_INIT_MUTEX);
		exit(ERR_INIT_MUTEX);
	}
#endif
	/* Init Barrier */
#ifdef CUSTOM_BARRIER
	gs32remain = s32threadcnt;
	if(0 != pthread_mutex_init(&gmutex_barrier, NULL))
	{
		print_error(ERR_INIT_MUTEX);
		exit(ERR_INIT_MUTEX);
	}
	if(0 != pthread_cond_init(&gcond_var, NULL))
	{
		print_error(ERR_INIT_MUTEX);
		exit(ERR_INIT_MUTEX);
	}

#elif CUSTOM_SPIN_BARRIER

	gs32MAX_THREAD = s32threadcnt;

#else
	if(0 != pthread_barrier_init(&gstrbar,
								 NULL,
								 s32threadcnt))
	{
		print_error(ERR_SET_BARRIER);
		exit(ERR_SET_BARRIER);
	}
#endif

	/* create threads and passing parameter */
	for(s32i = 0; s32i < gstrArg.s32pthpercore; s32i++)
	{
		for(s32j = 0; s32j < s32PCPUcnt; s32j++)
		{
			s32idx = s32i * s32PCPUcnt + s32j;

			/* Create and init an attribute structure */
			s32prio   = (s32i < (gstrArg.s32pthpercore / 2))
					  ? gstrArg.as32prio[0]
					  : gstrArg.as32prio[1];
			pstrworkarg[s32idx].s32prio = s32prio;
			pstrworkarg[s32idx].pstrprofarg = &gstrArg;

			set_pthread_schedinfo_attr(&pstrattr[s32idx],
										gstrArg.s32sched_policy,
										s32prio,
										s32j);

			/* Use this attribute to create pthreads */
			if(0 != pthread_create(&ppthread[s32idx],
						   &pstrattr[s32idx],
						   work_fxn,
						   (void*)&pstrworkarg[s32idx]))
			{
				print_error(ERR_THREAD_CREATE);
				exit(ERR_THREAD_CREATE);
			}
		}
	}


	/* Wait Threads Finishes and Get them synchronized */
	for(s32i = 0; s32i < gstrArg.s32pthpercore; s32i++)
	{
		for(s32j = 0; s32j < s32PCPUcnt; s32j++)
		{
			s32idx = s32i * s32PCPUcnt + s32j;
			pthread_join(ppthread[s32idx], NULL);
		}
	}


	/* Remeber to free mem, Destroy Instance */
#ifdef CUSTOM_BARRIER
	if(0 != pthread_cond_destroy(&gcond_var))
	{
		print_error(ERR_DES_MUTEX);
		exit(ERR_DES_MUTEX);
	}
	if(0 != pthread_mutex_destroy(&gmutex_barrier))
	{
		print_error(ERR_DES_MUTEX);
		exit(ERR_DES_MUTEX);
	}
#elif CUSTOM_SPIN_BARRIER

#else
	if(0 != pthread_barrier_destroy(&gstrbar))
	{
		print_error(ERR_DES_BARRIER);
		exit(ERR_DES_BARRIER);
	}
#endif

#ifdef USE_SPINLOCK
	if(0 != pthread_spin_destroy(&gspin_cabinidx))
	{
		print_error(ERR_DES_MUTEX);
		exit(ERR_DES_MUTEX);
	}
#else
	if(0 != pthread_mutex_destroy(&gmutex_cabinidx))
	{
		print_error(ERR_DES_MUTEX);
		exit(ERR_DES_MUTEX);
	}
#endif

	for(s32i = 0; s32i < gstrArg.s32pthpercore; s32i++)
	{
		for(s32j = 0; s32j < s32PCPUcnt; s32j++)
		{
			s32idx = s32i * s32PCPUcnt + s32j;
			pthread_attr_destroy(&pstrattr[s32idx]);
		}
	}

	free(pstrworkarg);
	free(pstrattr);    
	free(ppthread);
	free(gstrArg.ps32cabin);
	return 0;
}
