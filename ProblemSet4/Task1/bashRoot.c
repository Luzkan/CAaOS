#include <unistd.h>

//https://stackoverflow.com/questions/34052094/how-to-run-bash-with-root-rights-in-c-program

int main()
{
	// https://forum.pasja-informatyki.pl/201481/int-main-int-argc-char-argv
	// first element is the name of the program
	// second, third and so on are the arguments
	char *argv[1] = {NULL};

	//https://en.wikipedia.org/wiki/Setuid
	// 0 -> root
	setuid(0);
	execvp("/bin/bash", argv);

	return 0;
}

//sudo chown root.root a.out
//sudo chmod +s a.out