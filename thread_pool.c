#include <stdio.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


pthread_mutex_t flaglock;
pthread_cond_t mycond;
typedef void (*p)(char *a[]);


typedef struct {
	p call;
	char *arg[10];
}fun_arg;
typedef struct node{
	fun_arg *fuar_p;
	struct node *next;
}mytar,*mytar_p;

typedef struct {
	mytar *front;
	mytar *rear;
}queue,*listp_queue;



listp_queue g_queue;

listp_queue creat_queue()
{
	listp_queue myqueue = (listp_queue)malloc(sizeof(queue));
	mytar_p amytar = (mytar_p)malloc(sizeof(mytar));
	amytar->next = NULL;
	myqueue->front = myqueue->rear = amytar;
	return myqueue;
}

int queue_in(listp_queue myqueue, fun_arg *myfun)
{
	mytar_p newtar = (mytar_p)malloc(sizeof(mytar));
	newtar->fuar_p = myfun;
	myqueue->rear->next = newtar;
	newtar->next = NULL;
	myqueue->rear = myqueue->rear->next;
	return 0;
}	

fun_arg *queue_out(listp_queue myqueue)
{	
	if(myqueue->front == myqueue->rear)
		return NULL;

	mytar_p temp = myqueue->front;
	myqueue->front = myqueue->front->next;
	free(temp);					//add this cause segmention fault.......
	return myqueue->front->fuar_p;
}

void myprintf(char *str[])
{
	int i = 0;
	while(str[i++])
	{
		printf("%s  ",str[i-1]);
		free(str[i-1]);
	}
	printf("\n");
}


void *mycall(void *arg)
{
	fun_arg *temp;
	while(1){
		pthread_mutex_lock(&flaglock);
		while(NULL == (temp = queue_out(g_queue)))
		{
//			printf("%lu,%p-stock\n",pthread_self(),&temp);
			pthread_cond_wait(&mycond, &flaglock);
		}
		pthread_mutex_unlock(&flaglock);
		if(temp)
		{
			printf("%lu,----------temp:%d\n", pthread_self(),(unsigned int)(&temp)/8/1024/1024-360);
			printf("%p,:%p\n", temp->call,temp->arg);
			temp->call(temp->arg);		//got a problem when too many thread .
			sleep(1);
		}
	}
}
#if 0
void *mycall1(void *arg)
{
	mytar_p temp;
	printf("%lu,comeing\n",pthread_self(),&temp);
	while(1){
		pthread_mutex_lock(&flaglock);
		while(NULL == (temp = queue_out(g_queue)))
		{
			printf("%lu,%p-stock\n",pthread_self(),&temp);
			pthread_cond_wait(&mycond, &flaglock);
		}
		pthread_mutex_unlock(&flaglock);
		pthread_cond_broadcast(&mycond);
		printf("%lu,---------------temp:%p\n", pthread_self(),&temp);
		if(temp)
		{
			temp->call(temp->arg);
		}
	}
}

#endif


int main(int argc, char *argv[])
{
	g_queue = creat_queue();
	fun_arg *a = malloc(sizeof(fun_arg));
	a->call = myprintf;
	a->arg[0] = malloc(20);
	strncpy(a->arg[0],"aaaaabcdefg\n",8);
	a->arg[1] = NULL;
	fun_arg *b = malloc(sizeof(fun_arg));
	b->call = myprintf;
	b->arg[0] = malloc(20);
	strncpy(b->arg[0],"bbbbbbbbbbcdef\n",8);
	b->arg[1] = NULL;
	queue_in(g_queue,a);
	queue_in(g_queue,b);


	printf("stage1\n");
	int i = 0;
	pthread_t mypth[100];
	for(i = 0; i < 20; i++)
		pthread_create(&mypth[i],NULL,mycall,NULL);	
	char mm[8] = "Abcde";
	printf("stage2\n");
	while(1){
	i = 50;
	while(i--)
	{
//		printf("%d\n",i++);
		if(NULL == (a = malloc(sizeof(fun_arg))))
			perror("malloc");
		a->call = myprintf;
		if(mm[0]++ > 'z')
			mm[0] = 'A';
		
		if(NULL == (a->arg[0] = malloc(20)))
			perror("malloc");
		strncpy(a->arg[0],mm,9);
		if(NULL == (a->arg[1] = malloc(20)))
			perror("malloc");
		strncpy(a->arg[1],mm+1,9);
		a->arg[2] =  NULL;
		pthread_mutex_lock(&flaglock);
		queue_in(g_queue,a);
		pthread_mutex_unlock(&flaglock);
		pthread_cond_broadcast(&mycond);
		printf("queue in a number\n");
/*		
 *		b = malloc(sizeof(mytar));
		b->call = myprintf;
		if(mm[1]++ > 'z')
			mm[1] = 'A';
		strncpy(b->arg,mm+1,8);
		queue_in(g_queue,b);
		pthread_cond_broadcast(&mycond);
*/
	//	sleep(1);
	}
	sleep(2);
	}
	pause();
	return 0;
}
