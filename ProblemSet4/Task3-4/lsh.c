#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <fcntl.h>
#include <ctype.h>

// Marcel Jerzyk 02.12.2018, 23:52 @Wrocław hard-kucing, 11h straight, teh lulz
// https://brennan.io/2015/01/16/write-a-shell-in-c/
// this tutorial guided me trough the darkness and showed me the core steps
// that are needed to create our own shell
// added pipes
// added i/o
// added exit
// added ctrl + c interrupter
// also added my own "flavors" just to have fun with it
// added tons of descriptions to the code
// but then code starts to get messy because of deadline rush
// sadly, I had 24 hour cut from the time I prepared for it
// due to Hackathon and additional 12h to sleep back to full energy
// after the event
// there are few simple printf debuggers commented in "hard" part
char *lsh_read_line();
char **lsh_split_line(char *line);
bool lsh_detectamper(char *line);
int lsh_launch(char **args, bool amper);
int lsh_execute(char **args, bool amper);

//argument handling loop
void lsh_loop(void){

	//pointer to char line
	char *line;
	//bool for &
	bool amper;
	//pointer to a pointer to arguments
	char **args;
	int status;

	// do while guarantees atleast one execution
	do {
		//prompt
		printf("> ");
		//line reading
		line = lsh_read_line();
		//detect for &
		amper = lsh_detectamper(line);
		//splitting that line into arguments
		args = lsh_split_line(line);
		//finally - executing those arguments
		status = lsh_execute(args, amper);

		//https://www.tutorialspoint.com/c_standard_library/c_function_free.htm
		//deallocate memory
		free(line);
		free(args);
  	} while (status);
}

//argument read
char *lsh_read_line(void){
	char *line = NULL;
	// have getline allocate a buffer for us
	// https://jameshfisher.com/2017/02/22/ssize_t.html
	ssize_t bufsize = 0; 
	getline(&line, &bufsize, stdin);
	return line;
}

// end of line detector to look for &
bool lsh_detectamper(char* line)
{
	bool ampersyf;
	char* non_spaced = line;

	while(*line != '\0'){
		if(*line == '&'){
			printf("\e[91mlsh\e[0m Skip wait with & arg\n");
			ampersyf = true;
			*line = ' ';
			break;
		}
		line++;
	}
	/*
	if(*line == '&'){
		printf("congratz, detected &");
		ampersyf = true;
	}else{
		ampersyf = false;
	}*/
	return ampersyf;
}

// line splitter
char **lsh_split_line(char *line){
	int bufsize = 64, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "\e[91mlsh\e[0m: allocation error\n");
		exit(EXIT_FAILURE);
	}

	// strtok returns a pointer to first token
	// so this will give us pointers to the arguments
	// and all of them will end iwth \0 at the end
	// repeats until no token is returned

	token = strtok(line, " \t\r\n\a");
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += 64;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "\e[91mlsh\e[0m: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, " \t\r\n\a");
	}
	tokens[position] = NULL;
	return tokens;
}


/* There are two ways of starting procees
	1. Being Init
		- on boot when kernel is loaded and initialized
		- this process is managing load up of all the other process
	2. fork() system call with exec()
		- fork creates parent (return 0 to child) and child (return pid to parent)
		- exec replaces current process with entirely new one
			(process never returns from exec [unless error])
*/

int lsh_launch(char **args, bool amper){

	pid_t pid, wpid;
	int status;

	//Saves return value from fork
	pid = fork();

	if (pid == 0) {

		// This is the child process
		// https://www.systutorials.com/docs/linux/man/3-execvp/
		// Expects name and an array (string with arguments)
		// 'p' gives us freedom of not specifing path
		// it will look itself for the program name
		// as said before - getting any return from exec means
		// that there was na error
	
		if (execvp(args[0], args) == -1) {
			//printf("Debugger Test (execvp) #1\n");
			perror("\e[91mlsh\e[0m");
    	}
    	exit(EXIT_FAILURE);
	} else if (pid < 0) {

		// Pid Cannot be less than 0
		// Should never happen unless
		// Error forking

    	perror("\e[91mlsh\e[0m");
  	} else {

    	// This is the parent process
		// We need to wait for the child to execute
		// so we wait untill the child is killed or exited
		if(!amper){
			do {
				wpid = waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
  	}

	// Prompt for input again
	return 1;
}

/*	 			== Build-ins ==
	Not all commands are programs.
	Some of them for example: just changes directory
		And it's a property, not a program.
	Another example is exit.
		We want to exit the process we are in
		Not some new child one.
*/

int lsh_cd(char **args);
int lsh_exit(char **args);

// Build in commands as it stands in the task
char *builtin_str[] = {
	"cd",
	"exit"
};

// Array of function pointers
int (*builtin_func[]) (char **) = {
	&lsh_cd,
	&lsh_exit
};

int lsh_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}


int lsh_cd(char **args){

	// Checks if path exist
	if (args[1] == NULL) {
		fprintf(stderr, "\e[91mlsh\e[0m: expected argument to \"cd\"\n");
	} else {
		// Check for error
		if (chdir(args[1]) != 0) {
			perror("\e[91mlsh\e[0m");
		}
	}
	return 1;
}

// Of course - 0 to terminate
int lsh_exit(char **args){
  return 0;
}

//				==					==
//	 			== End of Build-ins ==
//				==					==



// parse input, and concatentate commands belongin to a process
int lsh_parse(char **input, char **commands, int pipe_count, int inputLen){
	int i = 0, j = 0;
	//printf("Debugger Test Pipe Amount #2\n");
	while(i < (pipe_count + 1)){
		for(; j < inputLen; j++){
		if(strcmp(input[j], "|") != 0){
			strcat(commands[i], input[j]);
			if(!((j + 1) >= inputLen)){
			strcat(commands[i], " ");
			}
		}else{
			j++;
			break;
		}
		}
		i++;
	}
	return 1;
}

// function count the number of pipes within a string array
int pipecount(char **input, int len){
	//printf("Debugger Test Pipe Amount #1\n");
	int i = 0, count = 0;
	for(i = 0; i <len; i++){
		if(strcmp(input[i], "|") == 0){
		count+= 1;
		}
	}
	return count;
}

// function trims leading and trailing whitespaces
void trim(char *str){
    int i;
    int begin = 0;

    int end = strlen(str) - 1;

    while (isspace((unsigned char) str[begin]))
        begin++;

    while ((end >= begin) && isspace((unsigned char) str[end]))
        end--;

    // Shift all characters back to the start of the string array.
    for (i = begin; i <= end; i++)
        str[i - begin] = str[i];

    str[i - begin] = '\0'; // Null terminate string.
}

// checks for redirection and returns the case
int whichRedirection(char *command){
	char *out = strstr(command, ">");
	char *in = strstr(command, "<");

	if((out != NULL) && (in != NULL)){
		//both inut and output redirection
		return 3;
	}else if(out != NULL){
		//output redirection only
		return 2;
	}else if(in != NULL){
		//input redirection only
		return 1;
	}else{
		return -1;
	}
}

// counts number of whitespaces in a given input string
int whitespaceCount(char *in, int len){
	int i = 0;
	int count = 0;
	for(i = 0; i < len; i++){
		if(in[i] == ' '){
		count++;
		}
	}
	return count;
}


// takes an input array and returns a char ** that points to
// the array of strings delimited by a space
int stitch(char *in, char **out){
	int i = 1;
	char *temp, cpy[100];

	sprintf(cpy, "%s", in);
	
	temp = strtok(in, " ");
	sprintf(out[0], "%s", temp);

	while((temp = strtok( NULL, " ")) != NULL){
		sprintf(out[i], "%s", temp);
		i++;
	}
	out[i] = NULL;
	return i;
}


// implements redirection
int lsh_redirection(char *input, int state, bool amper){

	pid_t pid, wpid;
	int status;

	//Saves return value from fork
	pid = fork();

	if (pid == 0) {

		char cpy[1000], path[1000], command[1000], filename[1000], **args, *temp;
		int spacecount, i = 0, fd;

		sprintf(cpy, "%s", input);

		temp = strtok(cpy, " ");
		sprintf(path, "%s", temp);

		sprintf(cpy, "%s", input);

		if(state == 1){

			temp = strtok(cpy, "<");
			sprintf(command, "%s", temp);
			command[strlen(command) - 1] = '\0';
			temp[strlen(temp) - 1] = '\0';

			temp = strtok(NULL, "\0");
			sprintf(filename, "%s", temp);
			trim(filename);

			spacecount = whitespaceCount(command, strlen(command));

			args = calloc((spacecount)+2, sizeof(char *));
			for(i = 0; i < spacecount + 1; i ++){
				args[i] = calloc(strlen(command)+10, sizeof(char));
			}

			i = stitch(command, args);

			if((fd = open(filename, O_RDONLY, 0644)) < 0){
				perror("\e[91mlsh\e[0m: open error");
				return -1;
			}

			dup2(fd, 0);
			close(fd);

			//printf("Debugger Test (execvp) #2\n");
			execvp(path, (char *const *)args);
			perror("\e[91mlsh\e[0m: execvp error");
			_exit(EXIT_FAILURE);

		}else if(state == 2){

			temp = strtok(cpy, ">");
			sprintf(command, "%s", temp);
			command[strlen(command) - 1] = '\0';
			temp[strlen(temp) - 1] = '\0';

			temp = strtok(NULL, "\0");
			sprintf(filename, "%s", temp);
			trim(filename);

			spacecount = whitespaceCount(command, strlen(command));

			args = calloc((spacecount)+2, sizeof(char *));

			for(i = 0; i < spacecount + 1; i ++){
				args[i] = calloc(strlen(command)+10, sizeof(char));
			}

			i = stitch(command, args);

			if((fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0){
				perror("\e[91mlsh\e[0m: open error");
				return -1;
			}

			dup2(fd, 1);
			close(fd);
			
			//printf("Debugger Test (execvp) #3");
			execvp(path, (char *const *)args);
			perror("\e[91mlsh\e[0m: execvp error");
			_exit(EXIT_FAILURE);    
			

		}else if(state == 3){
			char filename2[1000];
			int fd2;

			temp = strtok(cpy, "<");
			sprintf(command, "%s", temp);
			command[strlen(command) - 1] = '\0';
			temp[strlen(temp) - 1] = '\0';

			temp = strtok(NULL, ">");
			sprintf(filename, "%s", temp);
			filename[strlen(filename) - 1] = '\0';
			temp[strlen(temp) - 1] = '\0';
			trim(filename);

			temp = strtok(NULL, "\0");
			sprintf(filename2, "%s", temp);
			trim(filename2);

			spacecount = whitespaceCount(command, strlen(command));

			args = calloc((spacecount)+2, sizeof(char *));
			for(i = 0; i < spacecount + 1; i ++){
				args[i] = calloc(strlen(command)+10, sizeof(char));
			}

			i = stitch(command, args);

			if((fd = open(filename, O_RDONLY, 0644)) < 0){
				perror("\e[91mlsh\e[0m: open error");
				return -1;
			}

			if((fd2 = open(filename2, O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0){
				perror("\e[91mlsh\e[0m: open error");
				return -1;
			}

			dup2(fd, 0);
			close(fd);

			dup2(fd2, 1);
			close(fd2);

			//printf("Debugger Test (execvp) #4\n");
			execvp(path, (char *const *)args);
			perror("execvp error");
			_exit(EXIT_FAILURE);

		}

	} else if (pid < 0) {

		// Pid Cannot be less than 0
		// Should never happen unless
		// Error forking

    	perror("\e[91mlsh\e[0m");
  	} else {

    	// This is the parent process
		// We need to wait for the child to execute
		// so we wait untill the child is killed or exited
		if(!amper){
			do {
				wpid = waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
  	}
	return 1;
}


// Pipe manager
int lsh_pipe_execute(char **in, int *pipecount, int i, bool amper){

		if(i == *pipecount - 1){

			// This is the last process
			int x, spacecount, c;
			char ** args, cpy[1000];
			sprintf(cpy, "%s", in[i]);

			spacecount = whitespaceCount(in[i], strlen(in[i]));
			
			args = calloc((spacecount)+2, sizeof(char *));
			for(c = 0; c < spacecount + 1; c++){
			args[c] = calloc(strlen(in[i])+10, sizeof(char));
			}
			stitch(cpy, args);

			if((x = whichRedirection(in[i])) < 0){
				//printf("Debugger Test (execvp) #5\n");
				execvp(args[0], args);
				perror("\e[91mlsh\e[0m: execvp error");
				_exit(EXIT_FAILURE);
			}

			lsh_redirection(in[i], x, amper);
			return 1;
		}
		
		if(i < *pipecount){
			int fd[2], x, status, spacecount, c;
			pid_t pid;
			char ** args, cpy[1000];

			if(pipe(fd) < 0){
				perror("\e[91mlsh\e[0m: pipe");
				_exit(EXIT_FAILURE);
			}

			if((pid = fork()) < 0){
				perror("\e[91mlsh\e[0m: fork");
				_exit(EXIT_FAILURE);
			}

			if(pid != 0){

				dup2(fd[1], 1);
				close(fd[0]);
				in[i+1] = NULL;

				sprintf(cpy, "%s", in[i]);

				spacecount = whitespaceCount(in[i], strlen(in[i]));

				args = calloc((spacecount)+2, sizeof(char *));
				for(c = 0; c < spacecount + 1; c++){
					args[c] = calloc(strlen(in[i])+10, sizeof(char));
				}
				stitch(cpy, args);

				if((x = whichRedirection(in[i])) < 0){
					//printf("Debugger Test (execvp) #6\n");
					execvp(args[0], args);
					perror("\e[91mlsh\e[0m: execvp error");
					_exit(EXIT_FAILURE);
				}

				lsh_redirection(in[i], x, amper);
				waitpid(-1, &status, 0); //wait for all child processes to finish

			}else{

				if(i != *pipecount-1){
					dup2(fd[0], 0);
				}
				close(fd[1]);
				i+=1;
				lsh_pipe_execute(in, pipecount, i, amper);
			}
		} 
	return 1;
}

// Function that will either launch the buildin or process
int lsh_execute(char **args, bool amper){
	int i;
	int argc = 0;

	// User didn't enter anything
	if (args[0] == NULL) {
		return 1;
  	}

	// This checks if an argument we entered is a buildin
	// If it is, then it runs it
	// Otherwise it's launched as a process after the loop

	for (i = 0; i < lsh_num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
    	}
  	} 

	for (int i = 0; args[i] != NULL; ++i) { 
     	 argc++;
	}

	// Counting amount of pipes used in arguments
	int numPipes = pipecount(args, argc);
    char **commands =  calloc(numPipes +1, sizeof(char*));
    for(i = 0; i < (numPipes +1); i++){
        commands[i] = calloc((2*argc + 1), sizeof(char));
    }
	lsh_parse(args, commands, numPipes, argc);

	if(numPipes == 0){

		// X is marking which redirection we are going to use (if any)
		int x = 0;
		// Reminder:
		// 	3 for both
		//	2 for >
		//	1 for <
		//	non otherwise

 		if((x = whichRedirection(commands[0])) < 0){
			 //printf("Debugger No Redir Test.\n");
			return lsh_launch(args, amper);
						
		}else{
			//printf("Debugger Redit Test.\n");
			lsh_redirection(commands[0], x, amper);
		}
	}

	for(i = 0; i < (numPipes + 1); i++){
		trim(commands[i]);
	}

	// We are dealing with pipes
	//printf("Debugger Pipe Detected Test.\n");
	
	int count = numPipes+1;

	pid_t pid, wpid;
	int status;

	//Saves return value from fork
	pid = fork();

	if (pid == 0) {

	lsh_pipe_execute(commands, &count, 0, amper);

	} else if (pid < 0) {

		// Pid Cannot be less than 0
		// Should never happen unless
		// Error forking

    	perror("\e[91mlsh\e[0m");
  	} else {

    	// This is the parent process
		// We need to wait for the child to execute
		// so we wait untill the child is killed or exited
		// sleep(1);
		// signal(SIGQUIT, SIG_IGN);
		// kill(pid, SIGQUIT);
		if(!amper){
			do {
				wpid = waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
  	}
	return 1;		
}

void start_signalset();

void sig_handler(int signo){
	if (signo == SIGINT){
		int pid = getpid();
		printf("\e[91mlsh\e[0m:  Received CTRL + C. Stopping...\n");
	}
}

void start_signalset() {
    if(signal(SIGINT, sig_handler) == SIG_ERR) {
		  exit(0);  
	}
}


#define clear() printf("\033[H\033[J")
int main()
{
	start_signalset();

	clear();
	printf("\e[1m\e[92mWelcome to \e[91mlsh\e[92m. Your own simple shell.\e[0m\n");
	// Command interpreter
	lsh_loop();
}