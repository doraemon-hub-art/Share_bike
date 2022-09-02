#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int wakeflag = 0;

void wake_handle(int sig) {
	wakeflag = 1;
}

int main(void) {
	pid_t pd;
	char c;

	pd = fork();
	if (pd == -1) {
		printf("fork error!\n");
		exit(1);
	} else if (pd == 0) {
		for(;;){
            sleep(5);
            //getppid()获取父进程id
		    kill(getppid(), SIGALRM);//给父进程发送SIGALRM信号
        }
	} else {
		struct sigaction act; 
		act.sa_handler = wake_handle;
		act.sa_flags = 0;
		sigemptyset(&act.sa_mask);//清空
		sigaction(SIGALRM,  &act, 0);//设置信号响应

		for(;;){
            pause(); //把该进程挂起，直到收到任意一个信号

            if (wakeflag) {
                printf("Alarm clock work!!!\n");
            }
        }
	}

	return 0;
}