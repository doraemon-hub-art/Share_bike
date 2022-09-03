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
	sigaddset(&act.sa_mask, SIGUSR1);//���ź�SIGUSR1���뵽�ź����μ���
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);//����SIGINTֹͣ�ź�
    //�յ�SIGINT�źţ������ʱ���յ�SIGUSR1�����ִ������ڽ���SIGUSR1�źŴ���

	act2.sa_handler = myhandle;
	sigemptyset(&act2.sa_mask);
	act2.sa_flags = 0;
	sigaction(SIGUSR1, &act, 0);//����SIGUSR1�ź�

	while (1) {

	}

	return 0;
}