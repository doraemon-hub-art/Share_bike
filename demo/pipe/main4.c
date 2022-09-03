#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void) {
	int fd[2];
	int ret;
	char buff1[1024];
	char buff2[1024];
	pid_t pd;

	ret = pipe(fd);
	if (ret !=0) {
		printf("create pipe failed!\n");
		exit(1);
	}

	pd = fork();
	if (pd == -1) {
		printf("fork error!\n");
		exit(1);
	} else if (pd == 0) {
		for(;;){
            bzero(buff2, sizeof(buff2));
            sleep(3);
            //close(fd[1]);
            //printf("test\n");
            //read(fd[0], buff2, 3);
            strcpy(buff1, "Dad!");    
            write(fd[1], buff1, strlen(buff1));
            //printf("kid process(%d) received information:%s\n", getpid(), buff2);
        }
	} else {
		for(int i = 0;i<5;i++){
            bzero(buff2, sizeof(buff2));
            strcpy(buff1, "Hello!");    
            close(fd[1]);
            write(fd[1], buff1, strlen(buff1));
            //sleep(10);
           
            read(fd[0], buff2, sizeof(buff2));
            //printf("dad process(%d) received information:%s\n", getpid(), buff2);
            //printf("dad process(%d) send information:%s\n", getpid(), buff1);
            printf("kid process(%d) received information:%s\n", getpid(), buff2);
            sleep(3);
        }
	}

	if (pd > 0) {
		wait();
	}
	
	return 0;	
}