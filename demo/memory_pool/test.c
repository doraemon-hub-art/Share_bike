#include <stdio.h>

int main(void){
    char c[] = "123";
    printf("%d\n",sizeof(c));//4
    c[1] = '\0';
    printf("%d\n",sizeof(c));//4
    printf("%s\n",c);//1
    return 0;
}