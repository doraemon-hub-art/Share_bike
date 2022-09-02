#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void) 
{
	int fd[2];
	int ret;
	char buff1[1024];
	char buff2[1024];
	pid_t pd;

	ret = pipe(fd);
	if (ret !=0) {
		printf("create pipe failed!\n");
		exit(1);
	}

	pd = fork();
	if (pd == -1) {
		printf("fork error!\n");
		exit(1);
	} else if (pd == 0) {
		//bzero(buff2, sizeof(buff2));
		//sprintf(buff2, "%d", fd[0]);
		close(fd[1]);

		close(0);//关闭标准输入文件描述符
		dup(fd[0]);//复制 fd[0] ，并且使用可用的最小的文件描述符作为此文件描述符
        //即，此子进程使用管道的读端替换标准输入文件描述符
		close(fd[0]);//关闭原来的读端
		
		execlp("./od.exe", "./od.exe", "-c", 0);
        //如果execlp执行成功，则下面不会执行
		printf("execl error!\n");
		exit(1);
	} else {
		close(fd[0]);//关闭读端
	
        //写
		strcpy(buff1, "Hello!");
		write(fd[1], buff1, strlen(buff1)); 
        printf("send...\n");
		close(fd[1]);//关闭写端
	}
	
	return 0;	
}