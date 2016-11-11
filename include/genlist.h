#ifndef _GENLIST_H_
#define _GENLIST_H_
typedef struct node{
	struct node *pre,*next;
}listnode;

int insert_listnode(listnode *head,listnode *newnode);

int delete_listnode(listnode *head);


#define container_of(memberptr, Xstruct, Xmember) \
		(     (Xstruct *)( (int)memberptr - (int)( &(((Xstruct*)0)->Xmember) ) )      )


#endif
