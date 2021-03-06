#ifndef _DDEBUG_
#define _DDEBUG_
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#ifdef ALLPRINT
#define D_ALLPRINT 0xffff
#else 
#define D_ALLPRINT 0
#endif

#ifdef URGENCY
#define D_URGENCY		 0x0001
#else
#define D_URGENCY		0
#endif

#ifdef WARING
#define D_WARING 		 0x0002
#else
#define D_WARING 		0
#endif

#ifdef REMINDER
#define D_REMINDER		 0x0004
#else
#define D_REMINDER 		0
#endif

#if defined(ALLPRINT) || defined(URGENCY) || defined(WARING) || defined(REMINDER)
	unsigned int debug_print_flags = D_URGENCY | D_WARING | D_REMINDER | D_ALLPRINT;
#define DDEBUG(flag, fmt, ...) \
	 do{\
		 if(debug_print_flags & flag){\
			 printf(fmt, __VA_ARGS__);}\
	 }while(0)
#else
#define DDEBUG(fmt, ...) 


#endif


#ifdef SYSERR_CONT
#define syserr(arg...) do{ 	\
	fprintf(stderr,arg);	\
	fprintf(stderr,": %s\n",strerror(errno));\
}while(0)
#else
#define syserr(arg...) do{ 	\
	fprintf(stderr,arg);	\
	fprintf(stderr,": %s\n",strerror(errno));\
	exit(1);				\
}while(0)
#endif


#endif
