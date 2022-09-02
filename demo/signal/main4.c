#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void myhandle(int sig) {
	printf("Catch a signal : %d\n", sig);
}

int main(void) {
	struct sigaction act;

	act.sa_handler = myhandle;
	sigemptyset(&act.sa_mask);//Çå¿ÕÆÁ±Î¼¯
    act.sa_flags = 0;

	sigaction(SIGINT, &act, 0);
	while (1) {
        sleep(1);
        printf("sleep 1 second.\n");
	}
	return 0;
} 