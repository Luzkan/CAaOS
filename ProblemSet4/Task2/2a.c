#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int signo)
{
	printf("|-| Handled:%d\n", signo);

	// Should react to CTRL+C or on second signal)
	// Just a test
	if (signo == SIGINT)
		printf("|-|-|-| Received SIGINT (test)\n");
}

int main(void)
{

	printf("|-|-|-|-| Test if loop trough all signals is possible.\n|-|\n");

	// https://www.geeksforgeeks.org/signals-c-language/
	// https://www.thegeekstuff.com/2012/03/catch-signals-sample-c-code/

	int sigNum = 1;
	while(sigNum < 64){
		signal(sigNum, sig_handler);
		sigNum++;
	}

	// https://www.geeksforgeeks.org/getppid-getpid-linux/

	sigNum = 1;
	int pid = getpid();
	while(sigNum < 64){
		printf("|-|-| Send: %d\n", sigNum);
		kill(pid, sigNum);
		sleep(1);
		sigNum++;
	}

	return 0;
}
