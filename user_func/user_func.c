#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "pthreadpool.h"

#include "ddebug.h"

#define MAX_LEN 1024

static int ack(int flag)
{
	int ret = 0;
	switch(flag)
	{
		case	1:
					printf("login sucess\n");
					ret = 1;
					break;
		case	2:
					printf("user is not exist\n");
					break;
		case	3:
					printf("password is not correct\n");
					break;
		case	4:
					printf("register sucess\n");
					ret = 1;
					break;
		case	5:
					printf("register failed\n");
					break;
		case	6:
					printf("upload file  is exist\n");
					break;
		case	7:
					printf("file is not exist\n");
					break;
		case	8:
					printf("file is delivered\n");
					ret = 1;
					break;
		case	9:
					printf("verify sucess\n");
					ret = 1;
					break;
		case	10:
					printf("verify failed\n");
					break;
		default:
					break;
	}
	return ret;
}



/**
 * [verify_user description]
 * @param user_psswd_buf [description]
 * @param sockfd         [description]
 * @param index          1 login, 4 ack
 */
int verify_user(char user_psswd[2][20], int sockfd, int id,int index)
{
	
	static char user_psswd_buf[2][20];
	char buf[100] = {0x01,  0x00,0x29,  0x01};
	buf[0] = id;
	buf[3] = index;


//	printf("%s\n",user_psswd[0]);
//	printf("%s\n",user_psswd[1]);
	
	if(user_psswd)
	{
//		printf("get password\n");
		strcpy(user_psswd_buf[0],user_psswd[0]);
		strcpy(user_psswd_buf[1],user_psswd[1]);

	}
//	printf("::%s\n",user_psswd_buf[0]);
//	printf("::%s\n",user_psswd_buf[1]);
		//打包验证数据
//		printf("password alardy get\n");

		int ret;
		if(id == 7) //验证用
		{
			strncpy(buf+3,user_psswd_buf[0],20);
			strncpy(buf+23,user_psswd_buf[1],20);
			ret = send(sockfd, buf, 43, 0);
		}
		else //登陆用
		{
			strncpy(buf+4,user_psswd_buf[0],20);
			strncpy(buf+24,user_psswd_buf[1],20);
			ret = send(sockfd, buf, 44, 0);
		}

		ret = recv(sockfd, buf, 4, MSG_WAITALL);

		ret = ack(buf[3]);
		return (ret && (4 == buf[0]));

}

/**
 * [split_user_cmd description]
 * @param myarg [description]
 * @param buf   [description]
 * @param i     [description]
 */
void split_user_cmd(char *myarg[], char buf[100],int *i)
{
//	printf("%s",buf);
	char *rear,*head;
	*i = 0;
	rear = head =buf;
		while(*rear != '\0')
		{
			while(*head == ' ' && *head != '\0')
			{
				head++;
			}
			if(*head == '\0')
				break;
			rear = head;
			while(*rear != ' ' && *rear != '\0')
			{
				rear++;
			}
			strncpy(myarg[(*i)++],head,rear-head);
			myarg[*i-1][rear-head] = 0;
//			printf("get:%s\n",myarg[(*i)-1]);
			head = rear;
		}
		strncpy(myarg[*i]," ",2);
}



int socket_create(char *myarg[])
{
	// create socket 创建套接字
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sockfd)
		syserr("sockfd");

	static	struct sockaddr_in serveraddr = {0};
	static int len = sizeof serveraddr;
	if(myarg != NULL)
	{
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(atoi(myarg[2]));
		serveraddr.sin_addr.s_addr = inet_addr(myarg[1]);//IPv4
	}

	if(-1 == connect(sockfd, (struct sockaddr*)&serveraddr, len))
		syserr("connect");

	return sockfd;
}

//用户登录注册
void login(int argc, char *myarg[])
{

	//char buf[100] ={0};
	if(3 != argc)
	{
		printf("Usage: %s <IP> <PORT> \n", myarg[0]);
	}

	// int sockfd = socket_create(myarg);
	int sockfd ;
	int flag;
	char user_psswd_buf[2][20];

		while(1)
		{
			sockfd = socket_create(myarg);
			printf("1 for login or 2 for register \n");fflush(stdout);
			char c = getchar();

			while(getchar()!='\n');
			if ( 0 == strcmp(&c,"1"))
			{
				printf("name> ");fflush(stdout);
				gets(user_psswd_buf[0]);

				printf("password> ");fflush(stdout);
				gets(user_psswd_buf[1]);
				flag = verify_user(user_psswd_buf, sockfd,1, 1);
				if (!flag)
				{
					continue;
				}

				break;
			}
			else if ( 0 == strcmp(&c,"2"))
				{
					printf("name> ");fflush(stdout);
					gets(user_psswd_buf[0]);

					printf("password> ");fflush(stdout);
					gets(user_psswd_buf[1]);
					flag = verify_user(user_psswd_buf, sockfd,1, 3);
					if (!flag)
					{
						continue;
					}

					break;
				}
			else
				{
					printf("argument is not matching!\n ");
					continue;
				}

		}



	close(sockfd);

}


// 用户上传
void upload(  char *myarg_up)
{


	int sockfd = socket_create(NULL);

	int flag = verify_user(NULL, sockfd,7, 4);/********/
	if (!flag)
	{
		printf("passwd is not correct\n");
		return;
	}

	printf("in the upload\n");
/*	//验证完后，开始上传文件
	int num_up;
	char myarg_up[10][20];
	char buf_up[100] ={0};

	//用户输入 命令
	printf("myyun: upload_file> ");fflush(stdout);
	gets(buf_up);

	//分解用户命令
	split_user_cmd(myarg_up, buf_up, &num_up);
*/

	//////////////////////////////////////
	struct stat sb;

//	printf("file:%s\n",myarg_up);
	if (stat(myarg_up, &sb) == -1)
	{
	   syserr("stat");
	   exit(EXIT_FAILURE);
	}
	int filesize = (int)(sb.st_size);

//	printf("in the upload two\n");


	int src_filename_len = 50;
	int dest_filename_len = 50;
	//int dest_filename_len = strlen(myarg_up[2]);
	//int len = 4 + src_filename_len + dest_filename_len;
	int len = 104;

	char buf[107] = {0x02};
	//buf[1] = len / 256;
	//buf[2] = len % 256;

	buf[3] = filesize>>24;
	buf[4] = filesize>>16;
	buf[5] = filesize>>8;
	buf[6] = filesize>>0;
	//strncpy(buf+3, filesize ,4);//文件大小
	strncpy(buf+3+4,myarg_up,src_filename_len);//原文件名字
	//strncpy(buf+3+4+src_filename_len,myarg_up[2],dest_filename_len);//目标路径

//	printf("in the upload three\n");

	int ret = send(sockfd, buf, len + 3, 0);

	ret = recv(sockfd, buf, 4, MSG_WAITALL);
	ret = ack(buf[3]);
	if (!ret)
	{
		return;
	}


	//////读文件写数据
	int rdfd = open(myarg_up, O_RDONLY);

	while(filesize)
	{
		char file_buf[MAX_LEN] = {0x06};

		ret = read(rdfd, file_buf+3, MAX_LEN - 3);
//		printf("send:%d\n",ret);
		file_buf[1] = ret / 256;
		file_buf[2] = ret % 256;

		filesize -= ret;
		printf("remand:%d\n",filesize);
		ret = send(sockfd, file_buf, ret+3, 0);
	}
	//////////////////////////////////////


	ret = recv(sockfd, buf, 4, MSG_WAITALL);

	ret = ack(buf[3]);//看是否成功



	close(sockfd);
	close(rdfd);
}



// 用户下载
void download(  char *myarg)
{
	int sockfd = socket_create(NULL);
	printf("read to download\n");

	int flag = verify_user(NULL, sockfd,7, 4);/********/
	if (!flag)
	{
		printf("passwd is not correct\n");
		return;
	}

	//请求并等待服务器响应，然后下载数据


	//请求下载数据
	unsigned char buf[107] = {0x03,  0x00,0x68};
	strncpy(buf+7,myarg,50);
	int ret = send(sockfd, buf, 107, 0);

	printf("read to create %s\n",myarg);

	int wrfd = open(myarg, O_RDWR | O_CREAT | O_TRUNC,0666);
	ret = recv(sockfd, buf, 107, MSG_WAITALL);
	//int msglen = buf * 256 + buf[2];
	//int dataname_len = msglen -4;
	int filesize = (buf[3]<<24) + (buf[4]<<16) + (buf[5]<<8) +(buf[6]<<0);

	printf("get the filesize:%d\n", filesize);

//	if (107 == ret && 3 == buf[0]) //开始下载
//	{
		int send_data_num = filesize; /******/
		while(filesize)
		{
			ret = recv(sockfd, buf, 3, MSG_WAITALL);//接受3头字节
		//	printf("recv:%d\n",ret);
			if (3 != ret || 6 != buf[0])
			{
				syserr("wrong num");
			}
			char databuf[MAX_LEN] = {0};
			int datalen = buf[1] * 256 + buf[2];
			ret = recv(sockfd, databuf, datalen, MSG_WAITALL);
			printf("recv:%d\n",ret);
			write(wrfd, databuf, ret);

			filesize -= ret;

			printf("send: %d%%\r\n", (int)(filesize/send_data_num) );
		}
//	}

	close(sockfd);
	close(wrfd);
}



void dealcommand( char *cmdline[])
{
	int sockfd = socket_create(NULL);

//	printf("%s,,%s\n",cmdline[0],cmdline[1]);

	int flag = verify_user(NULL, sockfd,7, 4);/********/
	if (!flag)
	{
		printf("passwd is not correct\n");
		return;
	}

//	printf("in the cmdline\n");

	//命令请求
	int len = strlen(cmdline[1]) + 2;//""
	char buf[100] = {0x05};
	buf[1] = len / 256;
	buf[2] = len % 256;
	buf[3] = (!strcmp(cmdline[0],"ls"))?  1 :( (!strcmp(cmdline[0],"cd"))? 2 : 3 );
	strncpy(buf+4, cmdline[1], len);
	int ret = send(sockfd, buf, len+3, 0);

	//接收总体长度
	unsigned char databuf[MAX_LEN] = {0};
	ret = recv(sockfd, databuf, 3, MSG_WAITALL);//接受3头字节
	if (3 != ret || 8 != databuf[0])//8代表只接受空消息头信息
	{
		syserr("cmdline - wrong num ");
	}


/*	if (!strcmp(cmdline[0], "ls"))
	{*/

		//此处需要知道客户端那边协议是传几个字节的长度信息filesize
		int filesize = databuf[1] * 256 + databuf[2];
		while(filesize)
		{
			//接收单次的长度
			ret = recv(sockfd, databuf, 3, MSG_WAITALL);//接受3头字节
			if (3 != ret || 6 != databuf[0])
			{
				syserr("wrong num");
			}

			int datalen = databuf[1] * 256 + databuf[2];
			ret = recv(sockfd, databuf, datalen, MSG_WAITALL);
			//split_user_cmd(databuf)  //需要吗?????????????
			//write(wrfd, databuf, ret);

			filesize -= ret;

			printf("%s", databuf);  //  此处可能需循环显示 ？？？？？
		}
		putchar('\n');

#if 0
	}
	else if (!strcmp(cmdline[0], "cd"))
	{
		recv(sockfd, databuf, 4, MSG_WAITALL);//接受3头字节
		//ret = ack(buf[3]);
	}
	else//( !strcmp(cmdline[0], "mkdir") )
	{
		int ret = recv(sockfd, databuf, 4, MSG_WAITALL);//接受3头字节
		ret = ack(buf[3]);
	}
#endif
}

void choose( char *cmdline[], int num)
{
//	printf("%s,,%s\n",cmdline[0],cmdline[1]);
	//int sockfd = socket_create(NULL);
	if ( !strcmp(cmdline[0],"download") )
	{
		THDPL *mypool = creat_pool(10);
		if (num >= 3)
		{
			//int sockfd = socket_create(NULL);
			num = num - 2;

			while (num)//线判断还是？？？？
			{
				pool_add_task(mypool, download, (void *)cmdline[num--]);
			}
		}
		destory_pool(mypool);

	}

	else if(!strcmp(cmdline[0],"upload"))
	{
		THDPL *mypool = creat_pool(10);
		if (num >= 3)
		{
			//int sockfd = socket_create(NULL);
			num = num - 2;

			while (num)//线判断还是？？？？
			{
				pool_add_task(mypool, upload, (void *)cmdline[num--]);
			}
		}
		destory_pool(mypool);

	}

	else if(!strcmp(cmdline[0],"ls") | !strcmp(cmdline[0],"cd") | !strcmp(cmdline[0],"mkdir"))
	{
		//int sockfd = socket_create(NULL);
		dealcommand(cmdline);
	}
	else if(!strcmp(cmdline[0],"help")) 	
	{ 		
		printf("Usage:\n"); 		
		printf("Upload File: upload <file1> <file2> .... <file diretctory>\n"); 		
		printf("Download File: download <file1> <file2> .... <file diretctory>\n"); 		
		printf("Command File: ls <file1> <file2> .... <file diretctory>\n"); 	
	} 	
	else 	
	{ 		
		printf("print \"help\"  for help\n"); 	
	}


}

/*typedef struct{
	char *cmd;
	int sockfd;
}CMDARG_t;*/

//if(strcmp(myarg_up[0],"upload"));




















