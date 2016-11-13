/* profiling.h */
/* Data structures for Profiling Program */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include "profiling.h"

static error_entry gastrerrlist[] =
{
	{ERR_ARGMISS,       "Argument Missing!"},
	{ERR_NOSCHED,       "Policy SCHED_[RR/FIFO/OTHER] Only!"},
	{ERR_NPOSR,         "Round should be a Postive integer!"},
	{ERR_NPOSI,         "Iter should be a Postive integer!"},
	{ERR_THREAD_CREATE, "Fail to Create thread!"},
	{ERR_MALLOC,        "Memory Allocate Fail!"},
	{ERR_SET_POLICY,    "Set Scheduler for thread Error!"},
	{ERR_PIN_CORE,      "Pin thread on CPU Error!"},
	{ERR_SET_BARRIER,   "Init Barrier Error!"},
	{ERR_DES_BARRIER,   "Destroy Barrier Error!"},
	{ERR_INIT_MUTEX,    "Init Mutex Fail!"},
	{ERR_DES_MUTEX,     "Destroy Mutex Error!"},
	{ERR_CFGFILE,       "Open ext.cfg File Error!"},
	{ERR_SET_PRIORITY,  "Set Priority Error!"},
	{ERR_UNDEF,         "Undefined Error!"},
};

static int geterrlistlen(void)
{
	return (sizeof(gastrerrlist) / sizeof(error_entry));
}

char* geterrmsg(int s32eno)
{
	int s32i;
	for(s32i = 0; s32i < geterrlistlen(); s32i++)
	{
		if(gastrerrlist[s32i].s32eno == s32eno)
			break;	
	}

	if(s32i >= geterrlistlen())
	{
		s32i--;
	}
	return &gastrerrlist[s32i].acerrmsg[0];
}

void print_error(int s32eno)
{
	fprintf(stderr, "ERROR[%3d]: %s\n", s32eno, geterrmsg(s32eno));
	return;
}

void print_prof_arg(prof_arg* pstrarg)
{
	int s32i;
	switch(pstrarg->s32sched_policy)
	{
	case SCHED_OTHER:
		printf("Sched: SCHED_OTHER\n");
		break;
	case SCHED_RR:
		printf("Sched: SCHED_RR\n");
		break;
	case SCHED_FIFO:
		printf("Sched: SCHED_FIFO\n");
		break;
	default:
		printf("Sched: UNKNOWN\n");
		break;
	}


	printf("PthPerCore [%5d]\nPrio [%2d] [%2d]\nRound [%5d]\nIter  [%5d]\nDLen  [%5d]\nData List:",
		  pstrarg->s32pthpercore,
		  pstrarg->as32prio[0],
		  pstrarg->as32prio[1],
		  pstrarg->s32round,
		  pstrarg->s32iter,
		  pstrarg->s32cnt);

	for(s32i = 0; s32i < pstrarg->s32cnt; s32i++)
	{
		printf(" %d", pstrarg->ps32cabin[s32i]);
	}
	printf("\n");

	return;
}
