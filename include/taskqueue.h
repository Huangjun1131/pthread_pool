#ifndef _TASKQUEUE_H_
#define _TASKQUEUE_H_
#include "genlist.h"


typedef void (*p)(void *);

typedef struct tnode{
	p call;
	void *arg;
	listnode node;
}TASKNODE;

typedef struct queue {
	TASKNODE *front,*rear;
}TASK_QUEUE;

TASK_QUEUE *taskqueue_creat();

int taskqueue_in(TASK_QUEUE *queue, p call, void *arg);

TASKNODE *taskqueue_out(TASK_QUEUE *queue);

int taskqueue_isempty(TASK_QUEUE *queue);

#endif
