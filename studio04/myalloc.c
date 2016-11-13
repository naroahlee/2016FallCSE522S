#include <stdio.h>
#include <stdlib.h>

#define BUFFER_LEN (1000000)

int main(int argc, char* argv[])
{
	int s32i;
	char* pcTemp[10];
	for(s32i = 0; s32i < 10; s32i++)
	{
		pcTemp[s32i] = malloc(BUFFER_LEN * sizeof(char));
	}
	for(s32i = 0; s32i < 10; s32i++)
	{
		free(pcTemp[s32i]);
	}
	return 0;
}

