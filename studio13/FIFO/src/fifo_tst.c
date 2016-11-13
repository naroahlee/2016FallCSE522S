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
	int s32fd;
	char cbuf;
	if(0 != mkfifo("myfifo", S_IRUSR | S_IWUSR))
	{
		perror("fifo");
		exit(EXIT_FAILURE);
	}
	
	s32fd = open("./myfifo", O_RDONLY);
	if(s32fd < 0)
	{
		fprintf(stderr, "Open FIFO Error\n");
		exit(EXIT_FAILURE);
	}

	while(read(s32fd, &cbuf, 1) > 0)
	{
		write(STDOUT_FILENO, &cbuf, 1);
	}


	close(s32fd);
	return 0;
}
