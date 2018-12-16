#include <stdio.h>
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


int main()
{
	//r = red, g = green, b = blue
	int r = 0;
	int g = 0;
	int b = 0;
	int count = 0;
	//loopjump is set to 3 to reduce the time needed to get trough whole RGB combination 
	//85^3 instead of 255^3 iterations
	//but of course it can be set to any number depending on what we want to do with it
	int loopjump = 3;

	
	//This loop is better looking with mre rainbow'y effect because it doesnt jump from MAX to MIN

	for(r = 0; r<=255; r+=loopjump)
	{
		for(g = 0; g<=255; g+=loopjump)
		{
			for(b = 0; b<=255; b+=loopjump)
			{
				count++;
				printf ("\033[38:2:%d:%d:%dm Hello world! (number #%d) (color: r%d, g%d, b%d)\n", r, g, b, count, r, g, b);
			}
			g+=loopjump;
			for(b = 255; b>=0; b-=loopjump)
			{
				count++;
				printf ("\033[38:2:%d:%d:%dm Hello world! (number #%d) (color: r%d, g%d, b%d)\n", r, g, b, count, r, g, b);
			}
		}

		for(g = 255; g>=0; g-=loopjump)
		{
			for(b = 0; b<=255; b+=loopjump)
			{
				count++;
				printf ("\033[38:2:%d:%d:%dm Hello world! (number #%d) (color: r%d, g%d, b%d)\n", r, g, b, count, r, g, b);
			}
			g-=loopjump;
			for(b = 255; b>=0; b-=loopjump)
			{
				count++;
				printf ("\033[38:2:%d:%d:%dm Hello world! (number #%d) (color: r%d, g%d, b%d)\n", r, g, b, count, r, g, b);
			}
		}
	}

	//This one is not as good for the eye as the loop above


	/*
	for(r = 0; r<=255; r+=loopjump)
	{
		for(g = 0; g<=255; g+=loopjump)
		{
			for(b = 0; b<=255; b+=loopjump)
			{
				count++;
				printf ("\033[38:2:%d:%d:%dm Hello world! (number #%d) (color: r%d, g%d, b%d)\n", r, g, b, count, r, g, b);
			}
		}
	}

	//Legacy code, testing stuff

	*/
	/*printf("%sred\n", KRED);
	printf("%sgreen\n", KGRN);
	printf("%syellow\n", KYEL);
	printf("%sblue\n", KBLU);
	printf("%smagenta\n", KMAG);
	printf("%scyan\n", KCYN);
	printf("%swhite\n", KWHT);
	printf("%snormal\n", KNRM);
	//printf ("\033[45;1m Testing something \n");
	//printf ("\033[48:2:255:0:255m Hello world\n");*/	


	return 0;
}

