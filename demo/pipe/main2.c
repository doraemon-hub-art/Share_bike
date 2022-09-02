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
        //子进程先读在写
		bzero(buff2, sizeof(buff2));
		read(fd[0], buff2, sizeof(buff2));//read在没收到数据时会阻塞
		printf("process(%d) received information:%s,buff2's address:%p\n", getpid(), buff2,buff2);
        
        sleep(5);
        strcpy(buff1, "Hello Dad!");
		write(fd[1], buff1, strlen(buff1)); 

	} else {
        //父进程先写再读
		strcpy(buff1, "Hello Kid");
		write(fd[1], buff1, strlen(buff1)); 
        sleep(5);
        
        bzero(buff2, sizeof(buff2));
        read(fd[0], buff2, sizeof(buff2));
		printf("process(%d) received information:%s,buff2's address:%p\n", getpid(), buff2,buff2);

	}

	if (pd > 0) {
		wait();
	}
	
	return 0;	
}