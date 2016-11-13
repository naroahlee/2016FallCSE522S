#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char* argv[])
{
	FILE* fd;
	FILE* fd_w;
	int s32temp;
	if(0 != mkfifo("myfifo", S_IRUSR | S_IWUSR))
	{
		perror("fifo");
		exit(EXIT_FAILURE);
	}
	
	fd = fopen("./myfifo", "r");
	if(NULL == fd)
	{
		fprintf(stderr, "Open FIFO Error\n");
		exit(EXIT_FAILURE);
	}

	fd_w = fopen("./myfifo", "w");
	if(NULL == fd_w)
	{
		fprintf(stderr, "Open FIFO Error\n");
		exit(EXIT_FAILURE);
	}

	do
	{
		fscanf(fd, "%d", &s32temp);
		printf("Old [%4d] New [%4d]\n", s32temp, s32temp * 2);
	} while(1);

	fclose(fd);
	fclose(fd_w);
	return 0;
}
