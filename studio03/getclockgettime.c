/* getclockgettime.c */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
	struct timespec strTemp;
	struct timespec strBefore;
	struct timespec strAfter;
	(void)clock_gettime(CLOCK_MONOTONIC, &strBefore);
	(void)clock_gettime(CLOCK_MONOTONIC, &strTemp);
	(void)clock_gettime(CLOCK_MONOTONIC, &strAfter);
	printf("Before : Time sec = %5ld \t nsec = %5ld\n", strBefore.tv_sec, strBefore.tv_nsec);
	printf("After  : Time sec = %5ld \t nsec = %5ld\n", strAfter.tv_sec, strAfter.tv_nsec);
	printf("Differ : nsec = %5ld\n", (long)((strAfter.tv_sec - strBefore.tv_sec) * 10e9L) + (strAfter.tv_nsec - strBefore.tv_nsec));
	return 0;
}
