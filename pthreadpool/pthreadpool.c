#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "pthreadpool.h"
#include <pthread.h>

static void *mycall(void *arg)
{
	TASKNODE *temp;
	THDPL *pool = (THDPL *)arg;

//	printf("wait \n");
	while(1)
	{
		pthread_mutex_lock(&(pool->flaglock));
		while(NULL == (temp = taskqueue_out(pool->queue)))
		{
			//printf("I'm ready to get task:%lu\n",pthread_self()/1024/1024/8-360);
			if(0 == pool->destoryflag)
			{
		//		printf("flag is on, ready to return \n");
				pthread_mutex_unlock(&(pool->flaglock));
				return 0;
			}
			pthread_cond_wait(&(pool->mycond), &(pool->flaglock));
		}
		pthread_mutex_unlock(&(pool->flaglock));
		if(temp)
		{
			//printf("ready to do work:%lu\n", pthread_self()/1024/1024/8-360/*(unsigned int)(&temp)/8/1024/1024-330*/);
			temp->call(temp->arg);
		}
	}
}



THDPL *creat_pool(int num)
{
	THDPL *newpool = (THDPL *)malloc(sizeof(THDPL));
	newpool->pthread_num = num;	
	newpool->queue  = taskqueue_creat();
	newpool->destoryflag = 1;			//发现一个问题，当把这句话设置放到线程创建之后，会死得很惨
	pthread_mutex_init(&(newpool->flaglock),NULL);
	pthread_cond_init(&(newpool->mycond),NULL);
	newpool->mypth = (pthread_t *)malloc(sizeof(pthread_t *) * num);
	while(num--)
		pthread_create(&(newpool->mypth[num]),NULL, mycall,(void *)newpool);	
	return newpool;
}



int pool_add_task(THDPL *pool, p call, void *arg)
{
	pthread_mutex_lock(&(pool->flaglock));
	taskqueue_in(pool->queue,call,arg);
	pthread_mutex_unlock(&(pool->flaglock));
	pthread_cond_signal(&(pool->mycond));
//	printf("an task is added\n");
	return 0;
}


int destory_pool(THDPL *pool)
{
	pool->destoryflag = 0;
	pthread_cond_broadcast(&(pool->mycond));
	while((pool->pthread_num)--)
	{
		pthread_join(pool->mypth[pool->pthread_num],NULL);
		//printf("%d:destory %lu\n",pool->pthread_num+1,pool->mypth[pool->pthread_num]/1024/1024/8-360);
	}
//	printf("thread pool is destoryed\n");
}


