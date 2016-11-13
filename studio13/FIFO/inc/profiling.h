/* profiling.h */
/* Data structures for Profiling Program */

#ifndef __PROFILING_H__
#define __PROFILING_H__

#define ERR_ARGMISS        (-1)
#define ERR_NOSCHED        (-2)
#define ERR_NPOSR          (-3)
#define ERR_NPOSI          (-4)
#define ERR_THREAD_CREATE  (-5)
#define ERR_MALLOC         (-6)
#define ERR_SET_POLICY     (-7)
#define ERR_PIN_CORE       (-8)
#define ERR_SET_BARRIER    (-9)
#define ERR_DES_BARRIER    (-10)
#define ERR_INIT_MUTEX     (-11)
#define ERR_DES_MUTEX      (-12)
#define ERR_CFGFILE        (-13)
#define ERR_SET_PRIORITY   (-14)
#define ERR_UNDEF          (-99)

typedef struct error_entry_t
{
	int s32eno;
	char acerrmsg[40];
} error_entry;

typedef struct prof_arg_t
{
	int s32pthpercore;		/* # of threads per core */
	int as32prio[2];		/* Two priorities */
	int s32sched_policy;	/* SCHED_RR, SCHED_FIFO, SCHED_NORMAL */
	int s32round;			/* round time per execution */
	int s32iter;			/* iteration time per round */
	int s32cnt;				/* length of cabin */
	int* ps32cabin;			/* pick int from cabin */
} prof_arg;

typedef struct work_arg_t
{
	int s32prio;
	prof_arg* pstrprofarg;
} work_arg;

void print_error(int s32eno);
void print_prof_arg(prof_arg* pstrarg);

#endif

