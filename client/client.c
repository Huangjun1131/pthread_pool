#include <stdio.h>

#include "pthreadpool.h"
#include "user_func.h"

int main(int argc, char  *argv[])
{

	int num;
	char *myarg[10];
	char buf[100] ={0};
	char aaaa[10][20];
	for(num =0;num < 10;num++)
		myarg[num] = aaaa[num];

	//login 登陆函数
	login(argc, argv);


	while(1)
	{
		//用户输入 命令
		//upload  download ls
		printf("myyun> ");fflush(stdout);
		gets(buf);

		//分解用户命令
		split_user_cmd(myarg, buf, &num);


		choose(myarg, num);

	}

}




