#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void myhandle(int sig) 
{
	printf("Catch a signal : %d\n", sig);
	int i;
	for (i=0; i<10; i++) {
		sleep(1);
}
	printf("Catch end.%d\n", sig);
}

int main(void) 
{
	struct sigaction act, act2;

	act.sa_handler = myhandle; 
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGUSR1);//将信号SIGUSR1加入到信号屏蔽集中
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);//处理SIGINT停止信号
    //收到SIGINT信号，如果此时再收到SIGUSR1，则会执行完后在进行SIGUSR1信号处理。

	act2.sa_handler = myhandle;
	sigemptyset(&act2.sa_mask);
	act2.sa_flags = 0;
	sigaction(SIGUSR1, &act, 0);//处理SIGUSR1信号

	while (1) {

	}

	return 0;
}