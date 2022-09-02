#include <stdio.h>
#include <stdlib.h>

int main(void){       
    int ret = 0;
    char buff[80] = {0,};
    
    //scanf从标准输入读——在本实例中，实际上从管道从来的
    ret = scanf("%s", buff);
    printf("[ret: %d]buff=%s\n", ret, buff);

    ret = scanf("%s", buff);
    printf("[ret: %d]buff=%s\n", ret, buff);//第二次scanf失败，返回-1
    return 0;
}