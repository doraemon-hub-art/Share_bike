#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE   1024

int main(void){
	FILE * file;
	char buff[BUFF_SIZE+1];
	int cnt;
	file = popen("./p2", "w");
	if (!file) {
		printf("fopen failed!\n");
		exit(1);
	}
	strcpy(buff, "hello world! i 'am 123456789testtest!!!");
	cnt = fwrite(buff, sizeof(char), strlen(buff), file);
	pclose(file);

	return 0;	
}