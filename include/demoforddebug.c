#include <stdio.h>
#include "ddebug.h"

int main(int argc, const char *argv[])
{
	char a[40] = "self define string";

	//use all DEBUG must with -DEBUG 
	//gcc with -DD_ALL for all printf
	
	DDEBUG(D_URGENCY, "it is urgency!!:%s\n",a); 	// gcc with -DDURGENCY
	
	DDEBUG(D_WARING,"it is waring!!:%s\n",a);		//gcc with -DDWARING
	
	DDEBUG(D_REMINDER,"it is reminder!!:%s\n",a);	//gcc with -DDREMINDER
				
	syserr("goodbey%s",a);		//general exit with exit(1);

	printf("afer a syserr!");		//execute while gcc with -DSYSERR_CONT
	return 0;
}
