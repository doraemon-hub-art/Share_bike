#include <stdlib.h>
#include <stdio.h>

int main(void) {
	int i;
pid_t pd = fork();
	for (i=0; i<5; i++) {
		
		/* ģ���ٽ���----begin */
		printf("Process(%d) In\n", getpid());		
		sleep(1);
		printf("Process(%d) Out\n", getpid());
         /* ģ���ٽ���----end */ 
		sleep(1);
	}
	return 0;
}