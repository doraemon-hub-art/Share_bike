#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void) 
{
	int fd[2];
	int ret;
	char buff1[1024];
	char buff2[1024];

	ret = pipe(fd);
	if (ret !=0) {
		printf("create pipe failed!\n");
		exit(1);
	}

	strcpy(buff1, "Hello!");
	write(fd[1], buff1, strlen(buff1)); //写进去一个hello
	printf("send information:%s\n", buff1);

	bzero(buff2, sizeof(buff2));
	read(fd[0], buff2, sizeof(buff2));//读出来hello
	printf("received information:%s\n", buff2);

	return 0;	
}