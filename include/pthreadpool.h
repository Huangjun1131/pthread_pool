#include "taskqueue.h"
#include <pthread.h>

typedef struct {
	int pthread_num;
	TASK_QUEUE *queue;
	pthread_mutex_t flaglock;
	pthread_cond_t mycond;
	int destoryflag;
	pthread_t *mypth;
}THDPL;

typedef struct {
	p call;
	void *arg;
}fun_arg;

THDPL *creat_pool(int num);

int pool_add_task(THDPL *pool, p call, void *arg);

int destory_pool(THDPL *pool);

