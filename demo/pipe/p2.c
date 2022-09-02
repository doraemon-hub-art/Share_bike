#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char* argv[]){
    int fd;
    char buff[1024] =  {'\0'};

    int cnt = read(0,buff,sizeof(buff));
    if(cnt > 0)buff[cnt] = '\0';
    printf("receive: %s\n",buff);
    return 0;
}