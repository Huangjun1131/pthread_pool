#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
		int *buf;
		int len;
	}node;
void *func1(void *node1)
{
	int i,temp;
	node *mynode;
	mynode = (node *)node1;
	while(1)
	
		for(i = 0; i < mynode->len/2; i++)
		{
			mynode->buf[i] ^= mynode->buf[mynode->len-1-i];
			mynode->buf[mynode->len-1-i] ^= mynode->buf[i];
			mynode->buf[i] ^= mynode->buf[mynode->len-1-i];
		}
	sleep(1);
}

void *func2(void *node1)
{
	int i;
	node *mynode;
	mynode = (node *)node1;
	while(1)
	{
		for(i = 0; i < mynode->len; i++)
		{
			printf("%d  ",mynode->buf[i]);
		}
		printf("\n");
		sleep(1);
	}
}
int main(int argc, char *argv[])
{

	int buf[10] = {0,1,2,3,4,5,6,7,8,9};
	pthread_t t1, t2;

	node node1,node2;
	node1.buf = buf;
	node2.buf = buf;
	node1.len = 10;
	node2.len = 10;

	pthread_create(&t1,NULL,&func1,&node1);
	pthread_create(&t2,NULL,&func2,&node2);
	pause();
	return 0;
}
