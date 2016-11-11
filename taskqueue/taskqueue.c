#include "taskqueue.h"
#include "genlist.h"
#include <stdlib.h>


TASK_QUEUE *taskqueue_creat()
{
	TASK_QUEUE *queue = (TASK_QUEUE *)malloc(sizeof(TASK_QUEUE));
	TASKNODE *temp = (TASKNODE *)malloc(sizeof(TASKNODE));
	queue->front = queue->rear = temp;
}


int taskqueue_isempty(TASK_QUEUE *queue)
{
	return (queue->front == queue->rear)?0:1;
}


int taskqueue_in(TASK_QUEUE *queue, p call, void *arg)
{	
	TASKNODE *node = (TASKNODE *)malloc(sizeof(TASKNODE));
	node->call = call;
	node->arg = arg;
	insert_listnode(&(queue->rear->node),&(node->node));
	queue->rear = node;
}

static TASKNODE *getnextlist(TASKNODE *old)
{
	return (TASKNODE *)container_of(old->node.next, TASKNODE, node);
}

TASKNODE *taskqueue_out(TASK_QUEUE *queue)
{
	if(!taskqueue_isempty(queue))	
		return NULL;
	TASKNODE *temp = queue->front;
	queue->front = getnextlist(queue->front);
	free(temp);
	return queue->front;
}
