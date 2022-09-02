#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


void myhandle(int sig) 
{
	printf("Catch a signal : %d\n", sig);
}

int main(void) 
{

	//设置信号处理函数
	signal(SIGINT, myhandle);//如果收到SIGINT信号，就执行myhandle,如果未设置信号处理函数，则进行默认处理——终止。
	while (1) {
            sleep(1);
	}
	return 0;
}
