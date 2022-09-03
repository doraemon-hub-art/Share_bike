#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

int wakeflag = 0;

void wake_handle(int sig) 
{
	wakeflag = 1;
}

int main(void) 
{
	int ret;
	
	struct sigaction act;
	act.sa_flags = 0;
	act.sa_handler = wake_handle;//������Ӧ����
	sigemptyset(&act.sa_mask);
	sigaction(SIGALRM, &act, 0);//�����ź�-��Ӧ
	
	printf("time =%ld\n", time((time_t*)0));

	ret = alarm(5);
	if (ret == -1) {
		printf("alarm error!\n");
		exit(1);
	}

	for(;;){
		//����ǰ���̣�ֱ���յ�����һ���ź�
		pause();

		if (wakeflag) {
			printf("wake up, time =%ld\n", time((time_t*)0));
		}
	}

	return 0;
}