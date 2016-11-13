/* parsing.c */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include "profiling.h"
#include "parsing.h"

int check_sched_policy(char* pcpolicy, int* ps32sched_policy)
{
	if(0 == strcmp(pcpolicy, "SCHED_RR"))
	{
		*ps32sched_policy = SCHED_RR;
	}
	else if(0 == strcmp(pcpolicy, "SCHED_FIFO"))
	{
		*ps32sched_policy = SCHED_FIFO;
	}
	else if(0 == strcmp(pcpolicy, "SCHED_OTHER"))
	{
		*ps32sched_policy = SCHED_OTHER;
	}
	else
	{
		return ERR_NOSCHED;
	}

	return 0;
}

int parsing_input(int argc, char* argv[], prof_arg* pstrarg)
{
	int s32ret = 0;
	int s32i;
	if(argc < 5)
	{
		return ERR_ARGMISS;
	}

	/* Sched Policy */
	s32ret = check_sched_policy(argv[1], &pstrarg->s32sched_policy);
	if(0 != s32ret)
	{
		return s32ret;
	}

	/* Round */
	pstrarg->s32round = atoi(argv[2]);
	if(pstrarg->s32round <= 0)
	{
		return ERR_NPOSR;
	}

	/* Iteration */
	pstrarg->s32iter = atoi(argv[3]);
	if(pstrarg->s32iter <= 0)
	{
		return ERR_NPOSI;
	}

	/* Length of Data List */
	pstrarg->s32cnt = argc - 4;
	pstrarg->ps32cabin = (int *)malloc(sizeof(int) * pstrarg->s32cnt);
	if(NULL == pstrarg->ps32cabin)
	{
		return ERR_MALLOC;
	}

	/* Data List */
	for(s32i = 0; s32i < pstrarg->s32cnt; s32i++)
	{
		pstrarg->ps32cabin[s32i] = atoi(argv[s32i + 4]);
	}

	return 0;
}

int parsing_cfgfile(prof_arg* pstrarg)
{
	FILE* fp;
	
	fp = fopen("exp.cfg", "r");	
	if(NULL == fp)
	{
		return ERR_CFGFILE;
	}

	fscanf(fp, "%d %d %d\n", &pstrarg->s32pthpercore, &pstrarg->as32prio[0], &pstrarg->as32prio[1]);
	fclose(fp);

	return 0;	
}
