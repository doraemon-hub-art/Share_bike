#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int workflag = 0;

void work_up_handle(int sig) {
	workflag = 1;
}

void work_down_handle(int sig) {
	workflag = 0;
}

int main(void) {
	pid_t pd;
	char c;

	pd = fork();
	if (pd == -1) {
		printf("fork error!\n");
		exit(1);
	} else if (pd == 0) {//子进程
		char *msg;
		struct sigaction act; 
		act.sa_flags = 0;

		act.sa_handler = work_up_handle;//结构体设置信号处理函数
		sigemptyset(&act.sa_mask);		
		sigaction(SIGUSR1, &act, 0);
		
		act.sa_handler = work_down_handle;
		sigaction(SIGUSR2, &act, 0);
		
		while (1) {
			if (!workflag) {
				msg = "child process work!";
			} else {
				msg = "CHILD PROCESS WORK!";
			}
			printf("%s\n", msg);
			sleep(1);
		}
		
	} else {//父进程
		while(1) {
			c = getchar();
			if (c == 'A') {
				kill(pd, SIGUSR1);//给子进程pd发送信号 SIGUSR1
			} else if (c == 'a') {
				kill(pd, SIGUSR2);//给子进程pd发送信号 SIGUSR2
			}
		}
	}
	
	return 0;
}