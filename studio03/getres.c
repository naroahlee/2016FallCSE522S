#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char* argv[])
{
	struct timespec strMyTspec;
	struct timespec strMyTspec2;
	(void)clock_getres(CLOCK_MONOTONIC, &strMyTspec);
	(void)clock_getres(CLOCK_MONOTONIC_COARSE, &strMyTspec2);
	printf("MONOTONIC       : Time sec = %5ld \t nsec = %5ld\n", strMyTspec.tv_sec, strMyTspec.tv_nsec);
	printf("MONOTONIC COARSE: Time sec = %5ld \t nsec = %5ld\n", strMyTspec2.tv_sec, strMyTspec2.tv_nsec);
	return 0;
}
