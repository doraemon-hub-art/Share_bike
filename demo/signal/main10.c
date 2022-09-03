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
	sigaction(SIGUSR1, &act, 0);//给当前进程安装SIGUSR1信号

	sigset_t proc_sig_msk;//当前的信号屏蔽字
	sigset_t old_mask;//旧的信号屏蔽字
	sigemptyset(&proc_sig_msk);//清空信号集
	sigaddset(&proc_sig_msk, SIGUSR2);//添加信号SIGUSR2到信号屏蔽集中
	sigprocmask(SIG_BLOCK, &proc_sig_msk, &old_mask);//修改进程的信号屏蔽字，以增加的方式。
	

	sigset_t tmp_sig_msk;//临时的信号屏蔽字——用于sigsuspend函数
	sigaddset(&tmp_sig_msk,SIGUSR1);
	while (1) {	
		printf("waitting...\n");
		sigsuspend(&tmp_sig_msk);//此时，当前进程无法响应SIGUSR1中
		printf("running...\n");
	}

	return 0;
}