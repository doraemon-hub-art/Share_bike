#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void myhandle(int sig) {
	printf("Catch a signal : %d\n", sig);
	printf("Catch end.%d\n", sig);
}

int main(void) {
	struct sigaction act, act2;

	act.sa_handler = myhandle;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGUSR1, &act, 0);//����ǰ���̰�װSIGUSR1�ź�

	sigset_t proc_sig_msk;//��ǰ���ź�������
	sigset_t old_mask;//�ɵ��ź�������
	sigemptyset(&proc_sig_msk);//����źż�
	sigaddset(&proc_sig_msk, SIGUSR2);//����ź�SIGUSR2���ź����μ���
	sigprocmask(SIG_BLOCK, &proc_sig_msk, &old_mask);//�޸Ľ��̵��ź������֣������ӵķ�ʽ��
	

	sigset_t tmp_sig_msk;//��ʱ���ź������֡�������sigsuspend����
	sigaddset(&tmp_sig_msk,SIGUSR1);
	while (1) {	
		printf("waitting...\n");
		sigsuspend(&tmp_sig_msk);//��ʱ����ǰ�����޷���ӦSIGUSR1��
		printf("running...\n");
	}

	return 0;
}