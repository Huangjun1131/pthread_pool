#include "genlist.h"


int insert_listnode(listnode *head,listnode *newnode)
{
	newnode->next = head->next;
	newnode->pre  = head;

	
	head->next = newnode;
	if(newnode->next)
		newnode->next->pre = newnode;
}

int delete_listnode(listnode *head)
{
	if(head->pre)
		head->pre->next = head->next;

	if(head->next)
		head->next->pre = head->pre;

}
