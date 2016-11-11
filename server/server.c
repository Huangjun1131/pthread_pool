#include <stdio.h>
#include "ddebug.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "serroutine.h"
#include "databasectl.h"
#include "pthreadpool.h"
pthread_mutex_t sqllock; 
int main(int argc, char *argv[])
{

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == listenfd)
		syserr("socket");

	pthread_mutex_init(&sqllock,NULL);
	struct sockaddr_in myaddr = {0};
	struct sockaddr_in clientaddr = {0};
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(8888);
	myaddr.sin_addr.s_addr = inet_addr("0.0.0.0");//INADDR_ANY
	int len = sizeof myaddr;

	if(-1 == bind(listenfd, (struct sockaddr*)&myaddr, len))
		syserr("bind");

	if(-1 == listen(listenfd, 10))
		syserr("listen");

	THDPL *poool = creat_pool(5);
	
	int epfd = epoll_create(2);
	struct epoll_event myevt = {0};
	myevt.events = EPOLLIN;
	myevt.data.fd = listenfd;
	
	struct epoll_event getevt = {0};
	getevt.events = EPOLLIN;
	getevt.data.fd = STDIN_FILENO;

	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &myevt);
	epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &getevt);

	struct epoll_event chevt[2] = {0};
	int sockfd;
	char buf[100] = {0};
	int nready;
	while(1)
	{
		if(-1 == (nready = epoll_wait(epfd, chevt, 2, -1)))
			syserr("epoll wait");
		while(nready--)
		{
			if((chevt[nready].data.fd == listenfd) && (chevt[nready].events & EPOLLIN))	
			{
				sockfd = accept(listenfd, (struct sockaddr*)&clientaddr, &len);
				if(-1 == sockfd)
					syserr("accept");
				DDEBUG("incoming: %s\n", inet_ntoa( clientaddr.sin_addr) );

				pool_add_task(poool,routine,(void *)sockfd);
			}
			if((chevt[nready].data.fd == STDIN_FILENO) && (chevt[nready].events & EPOLLIN))	
			{
				gets(buf);
				if(!strncmp(buf,"quit",4))
				{
					destory_pool(poool);
					close(listenfd);
					return 0;
				}
			}
		}
	}

	close(listenfd);
	return 0;
}
	
