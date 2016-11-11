#include "databasectl.h"
#include "serroutine.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
 #include <dirent.h>

extern pthread_mutex_t sqllock;
static int sendack(int socket, char type)
{
	char wbuf[4] = {0};
	wbuf[0] = 4;
	wbuf[1] = 0;
	wbuf[2] = 1;
	wbuf[3] = type;
	send(socket, wbuf, 4,0);
}

void routine(void *arg)
{
	unsigned char head[3]={0};
	char username[20]={0};
	char passwd[20]={0};
	unsigned char rbuf[1024] ={0};
	int filelen;
	int ret;

	int socket = (int)arg;
	//send from socket 3byte
	//
	ret = recv(socket,head,3,MSG_WAITALL);
	printf("%d,%d,%d,%d,,\n",ret,head[0],head[1],head[2]);
	
	int len = head[1];
	len = len<<8 + head[2];

	//switch cmmand
	switch(head[0])
	{
	//
	//case:verify()->download.upload.command?
		case 7:
			printf("ready to yanzheng\n");
			ret = recv(socket, rbuf, 40,MSG_WAITALL);
			strncpy(username, rbuf, 20);
			strncpy(passwd, rbuf+20, 20);
			//verify()
			
			pthread_mutex_lock(&sqllock); 
			if(0 == verify_user(username,passwd))
			{
				pthread_mutex_unlock(&sqllock); 
				sendack(socket, VERFAIL);
				close(socket);
				return ;
			}
			pthread_mutex_unlock(&sqllock); 
			//send ack;
			sendack(socket,VEROK);
			chdir(username);
			printf("verify ok,reply!\n");
//			send(socket, wbuf, 4);
			//get requirement
			recv(socket, rbuf, 3, MSG_WAITALL);

			printf("%d,%d,%d..\n",rbuf[0],rbuf[1],rbuf[2]);

			//download ??
			if(3 == rbuf[0])
			{
				unsigned char wbuf[1024] ={0};
				char filename[50]={0};
				printf("read to recevi download\n");
				recv(socket, rbuf, 104, MSG_WAITALL);
				//send filename and size
				strncpy(filename,rbuf + 4,50);
				struct stat buf;
				stat(filename,&buf);
				
				printf("read to downloadfile%s\n",filename);
				filelen = buf.st_size;

				wbuf[0] = 2;
				wbuf[1] = 0;
				wbuf[2] = 107;
				wbuf[3] = filelen>>24;
				wbuf[4] = filelen>>16;
				wbuf[5] = filelen>>8;
				wbuf[6] = (char)filelen;
				strncpy(wbuf+7,filename,50);

				len = 107; //????	
				send(socket, wbuf, len,0);
				
				
				int filefd = open(filename,O_RDONLY);
				head[0] = 6;
				while(1)
				{
					ret = read(filefd, rbuf, sizeof(rbuf)-3);
					if(0 == ret)
						break;
					head[1] = ret / 256;
					head[2] = ret % 256;
					send(socket, head, 3,0);
					send(socket, rbuf, ret,0);
					filelen-=ret;
						if(0 >= filelen )
						break;
				}
				close(socket);
				close(filefd);
				return;

			}
			//upload
			else if(2 == rbuf[0])
			{
				unsigned char wbuf[1024] ={0};
				char filename[100]={0};
				int buflen;
				recv(socket, rbuf, 104,MSG_WAITALL);

				//get filename
				strncpy(filename, rbuf+4, 50);
				printf("read to write file %s\n",filename);
				filelen = (rbuf[0]<<24)+(rbuf[1]<<16)+(rbuf[2]<<8)+rbuf[3];
				pthread_mutex_lock(&sqllock); 
				if(0 != file_isexist(username, filename))
				{
				pthread_mutex_unlock(&sqllock); 
					sendack(socket,FIEXIST);
					close(socket);
					return ;
				}
				pthread_mutex_unlock(&sqllock); 
				sendack(socket, VEROK);
				pthread_mutex_lock(&sqllock); 
				add_filename(username, filename, filelen);
				pthread_mutex_unlock(&sqllock); 

				printf("get filelen %d\n",filelen);
				//create file,if file exist,return 
				int filefd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
				printf("read to open file %s\n",filename);
				while(1)
				{
					ret = recv(socket, head, 3,MSG_WAITALL);
					printf("recv:%d\n",ret);
					if(0 == ret)
						break;
					buflen = head[1]*256+head[2];
					printf("redy to read %d,%d,%d\n",buflen,head[1],head[2]);
					ret = recv(socket, rbuf, buflen,MSG_WAITALL);
					printf("recv:%d\n",ret);
					
					write(filefd, rbuf, ret);
					filelen-=ret;
					if(0 >= filelen)
						break;
				}

				printf("upload ok\n");
				sendack(socket,VEROK);
				printf("send reply ok\n");
				close(socket);
				close(filefd);
				return ;

			}
			else if(5 == rbuf[0])
			{
				printf("%d,%d\n",rbuf[1],rbuf[2]);
				filelen = rbuf[1];
				filelen = filelen*256 + rbuf[2];
				printf("filelen:%d\n",filelen);
				recv(socket, rbuf, filelen, MSG_WAITALL);
				printf("get the line:%s\n",rbuf+1);
				switch(rbuf[0])
				{
					case 1:   //ls
						pthread_mutex_lock(&sqllock); 
						get_files(username, rbuf);
						pthread_mutex_unlock(&sqllock); 
						DIR *s_dir = opendir(".");
						struct dirent *mydir;
						strcat(rbuf,"\nIn this dir:\n");
						while(mydir = readdir(s_dir))
						{
							if(mydir->d_name[0] != '.')
								strcat(strcat(rbuf,mydir->d_name),"  ");
						}

						printf("get fils:%s\n",rbuf);
						filelen = strlen(rbuf) + 1;
						printf("get len is %d\n",filelen);
						head[0] = 8;
						head[1] = filelen / 256;
						head[2] = filelen % 256;
						send(socket, head, 3, 0);
						head[0] = 6;
						send(socket, head, 3, 0);
						send(socket, rbuf, filelen, 0);
		//				close(socket);
						return ;

						break;
					case 2: //cd
					//	pthread_mutex_lock(&sqllock); 
					//	get_files(username, rbuf);
					//	pthread_mutex_unlock(&sqllock); 
					//	printf("get fils\n");
					//	filelen = strlen(rbuf) + 1;
						if(0 == chdir(rbuf+1))
							sprintf(rbuf,"cd %s success\n",rbuf+1);
						else
							sprintf(rbuf,"cd %s failed\n",rbuf+1);


						printf("get len is %d\n",filelen);
						head[0] = 8;
						head[1] = filelen / 256;
						head[2] = filelen % 256;
						send(socket, head, 3, 0);
						head[0] = 6;
						send(socket, head, 3, 0);
						send(socket, rbuf, filelen, 0);
					//	close(socket);
						return ;


						break;
					case 3:  //mkdir
						if(0 == mkdir(rbuf+1,0777))
							strcpy(rbuf,"mkdir success\n");
						else
							strcpy(rbuf,"mkdir failed\n");

						filelen = strlen(rbuf) + 1;
						printf("get len is %d\n",filelen);
						head[0] = 8;
						head[1] = filelen / 256;
						head[2] = filelen % 256;
						send(socket, head, 3, 0);
						head[0] = 6;
						send(socket, head, 3, 0);
						send(socket, rbuf, filelen, 0);
					//	close(socket);
						return ;


						break;
				}
			}
			break;


		
	//login?
	//
	//
		case 1:
			printf("ready to login\n");
			ret = recv(socket,rbuf,41,MSG_WAITALL);
			printf("%d,%d,%s,%s,\n",ret,rbuf[0],rbuf+1,rbuf+21);
		//	ret = recv(socket, rbuf, rbuf[2],MSG_WAITALL);
	//		printf("%d,,%s\n",ret,rbuf);
			if(1 == rbuf[0])
			{
				printf("ready to denglu\n");
				strncpy(username, rbuf+1, 20);
				strncpy(passwd, rbuf+21, 20);
				//verify
				pthread_mutex_lock(&sqllock); 
				if(0 == verify_user(username,passwd))
				{
					pthread_mutex_unlock(&sqllock); 
					sendack(socket,PSWRO);
					return ;
				}
				pthread_mutex_unlock(&sqllock); 
				chdir(username);
				sendack(socket,LOGSS);
				
				return ;
			}
			else if(3 == rbuf[0])
			{
				printf("ready to zhuce\n");
				strncpy(username, rbuf+1, 20);
				strncpy(passwd, rbuf+21, 20);
				//verify not exist
				pthread_mutex_lock(&sqllock); 
				if(1 == user_isexist(username))
				{
					pthread_mutex_unlock(&sqllock); 
					sendack(socket,REFAIL );
					close(socket);
					return ;
				}
				pthread_mutex_unlock(&sqllock); 
				
				//add the user
				pthread_mutex_lock(&sqllock); 
				add_user(username,passwd);	
				create_filetable(username);
				pthread_mutex_unlock(&sqllock); 
				mkdir(username,0777);
				sendack(socket, REOK);
				close(socket);

				return;
				
			}
		}
}
