#include <stdio.h>
#include <stdlib.h>

int main(void){       
    int ret = 0;
    char buff[80] = {0,};
    
    //scanf�ӱ�׼����������ڱ�ʵ���У�ʵ���ϴӹܵ�������
    ret = scanf("%s", buff);
    printf("[ret: %d]buff=%s\n", ret, buff);

    ret = scanf("%s", buff);
    printf("[ret: %d]buff=%s\n", ret, buff);//�ڶ���scanfʧ�ܣ�����-1
    return 0;
}