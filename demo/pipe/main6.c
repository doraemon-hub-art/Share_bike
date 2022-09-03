#include <stdio.h>
#include <stdlib.h>
#define BUFF_SIZE   1024

int main(void){
	FILE * file;
	char buff[BUFF_SIZE+1];
	int cnt;

	// system("ls -l > result.txt");
	file = popen("ls -l", "r");//以读的方式去读取ls -l这个程序输出的结果 
	if (!file) {//判断是否打开成功
		printf("fopen failed!\n");
		exit(1);
	}

	cnt = fread(buff, sizeof(char), BUFF_SIZE, file);//以文件的方式读取内容，fread是从文件指针中读取。
	if (cnt > 0) {
		buff[cnt] = '\0';
		printf("%s", buff);
	}	
	pclose(file);//关闭

	return 0;	
}