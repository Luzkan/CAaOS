#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <linux/limits.h>

// http://www.firmcodes.com/write-printf-function-c/
// Basically had to understand what the va stuff is doing and write it "manually".
// gcc -m32 myprintf.c

void myprintf(char *,...); 	
int myscanf(const char*,...);			
char* convert(unsigned int, int);
int convertBack(char* string, int base);

int main(){ 
	myprintf("- These are few test args - \n12345 in decimal: %d", 12345); 
	//int userInputDecimal;
	//myscanf("%d", &userInputDecimal);
	//myprintf("%d", userInputDecimal);

    myprintf("29 in hexadecimal: %x", 29); 
	//int userInputHexadecimal;
	//myscanf("%x", &userInputHexadecimal);
	//myprintf("%x", userInputHexadecimal);

    myprintf("13 in binary: %b", 3); 
	//int userInputBinary;
	//myscanf("%b", &userInputBinary);
	//myprintf("%b", userInputBinary);
    
	int input = 12345;
    myprintf("input in decimal: %d", 12345);  

	return 0;
} 

void myprintf(char* theWholeThing,...){ 
	char *traverse; 
	unsigned int i; 
	char *s; 
	
	// Arg initialization
    char *p = (char *) &theWholeThing + sizeof theWholeThing;
	
	for(traverse = theWholeThing; *traverse != '\0'; traverse++){ 
        // We are lookin if there is any custom stuff that needs to be examined
        // They all start with %
		while( *traverse != '%' ){ 
			putchar(*traverse);
			traverse++; 
		} 
		traverse++; 
		
		// Fetching and executing arguments
		switch(*traverse){ 
            
            // Char
			case 'c' : i = *((int *)p);
						putchar(i);
						break; 
			// Decimal int
			case 'd' : i = *((int *)p); 		
						if(i<0){ 
							i = -i;
							putchar('-'); 
						} 
						puts(convert(i,10));
						break; 
			// String
			case 's': s = *((char **)p); 		
						puts(s); 
						break; 
			// Hexadecimal
			case 'x': i = *((unsigned int *)p); 
						puts(convert(i,16));
						break; 
            // Binary
            case 'b': i = *((unsigned int *)p); 
						puts(convert(i,2));
						break; 
		}	
	} 
	// Closing
    *p = '\0';
}


// This allows to make easy conversion to any base that we need.
char *convert(unsigned int num, int base) 
{ 
	static char Representation[]= "0123456789ABCDEF";
	static char buffer[50]; 
	char *ptr; 
	
	ptr = &buffer[49]; 
	*ptr = '\0'; 
	
	do{ 
		*--ptr = Representation[num%base]; 
		num /= base; 
	}while(num != 0); 
	
	return(ptr); 
}