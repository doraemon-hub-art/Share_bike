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
            //getppid()��ȡ������id
		    kill(getppid(), SIGALRM);//�������̷���SIGALRM�ź�
        }
	} else {
		struct sigaction act; 
		act.sa_handler = wake_handle;
		act.sa_flags = 0;
		sigemptyset(&act.sa_mask);//���
		sigaction(SIGALRM,  &act, 0);//�����ź���Ӧ

		for(;;){
            pause(); //�Ѹý��̹���ֱ���յ�����һ���ź�

            if (wakeflag) {
                printf("Alarm clock work!!!\n");
            }
        }
	}

	return 0;
}