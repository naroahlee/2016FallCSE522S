#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#define size 5000

volatile int i;

int called [size];

void sigint_handler( int signum){
	called[i] = 1;
	return;
}

int main (int argc, char* argv[]){
	struct sigaction ss;
	int s32temp;
	char acMsg[9] = "i:     \n";

	ss.sa_handler = sigint_handler;
	ss.sa_flags = SA_RESTART;

	sigaction( SIGINT, &ss, NULL );

	for(i = 0; i < size; i++){
		s32temp = i;

		acMsg[3] = '0' + s32temp / 1000;
		s32temp %= 1000;
		acMsg[4] = '0' + s32temp / 100;
		s32temp %= 100;
		acMsg[5] = '0' + s32temp / 10;
		s32temp %= 10;
		acMsg[6] = '0' + s32temp;

		write(0, acMsg, 9);
	}

	
	for(i = 0; i < size; i++){
		if( called[i] == 1 )
			printf("%d was possibly interrupted\n", i);
	}

	return 0;
}

