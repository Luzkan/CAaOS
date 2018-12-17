#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include <stdlib.h>

// sending multiple signals
// https://stackoverflow.com/questions/29805676/sending-multiple-sigusr1-sigusr2-signals

// singal que
// https://stackoverflow.com/questions/5285414/signal-queuing-in-c
// https://stackoverflow.com/questions/44664310/multiple-occurances-of-same-signal-at-signal-handler


void start_signalset();

void sig_handler(int signo){
	if (signo == SIGUSR1)
		printf("|-|   Received SIGUSR1\n");
		start_signalset();
}

void start_signalset() {
    if(signal(SIGUSR1, sig_handler) == SIG_ERR) {
		  exit(0);  
	}
}


int main(void){
	
	printf("|-|-|-|-| Let's send multiple SIGUSR1 to a process at one time w/ and w/o sleep.\n|-|\n");
	
    start_signalset();
	

	// https://www.geeksforgeeks.org/fork-system-call/

	int i = 1;
	int child;
	int pid = getpid();
	printf("|-| pid: %d\n", pid);


	if(child = fork() == 0){
		pid = getpid();
		printf("|-| Child:  %d, and his pid: %d | ", child, pid);
		pid--;
		printf("Now PID value will be equal to: %d\n", pid);
		
	}else{
		pid = getpid();
		printf("|-| Parent: %d, and his pid: %d | ", child, pid);
		pid++;
		printf("Now PID value will be equal to: %d\n", pid);
	}

	//sleep(2);
	
	for(i = 1; i <= 3; i++){
		signal(i, sig_handler);
	}

	//if(fork() == child){
	for(i = 1; i <= 3; i++){
		printf("|-|-| Sending 3 signals with sleep (#%d)\n", i);
		kill(pid, SIGUSR1);
		sleep(1);
	}

	printf("|-|-| Sending 10 signals w/o sleep\n");
	int k;
	while(k < 10){
		kill(pid, SIGUSR1);
		k++;
	}

	sleep(5);

	return 0;
}
