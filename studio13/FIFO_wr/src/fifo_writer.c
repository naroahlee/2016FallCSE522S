#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void useage(void)
{
	printf("Useage:\n");
	printf("\t./fifo_writer [0 (Even) | 1 (Odd)]\n");
}


int main(int argc, char* argv[])
{
	FILE* fd;
	int s32temp;
	int s32isodd = 0;

	if(2 != argc)
	{
		useage();
		exit(EXIT_FAILURE);
	}
	
	s32isodd = atoi(argv[1]);

	fd = fopen("./myfifo", "w");
	if(NULL == fd)
	{
		fprintf(stderr, "Open FIFO Error\n");
		exit(EXIT_FAILURE);
	}

	do
	{
		s32temp = rand() % 500 * 2;
		s32temp = (0 == s32isodd) ? s32temp 
								  : s32temp + 1;
		fprintf(fd, "%d\n", s32temp);
		fflush(fd);
		usleep(rand() % 10000 * 100);
	} while(1);

	fclose(fd);
	return 0;
}
