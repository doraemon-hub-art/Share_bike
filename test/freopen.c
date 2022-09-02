#include <stdio.h>
 
int main(void)
{
    FILE *out = freopen("stdout.txt", "w", stdout);
    printf("%s\n", "redirect test");
 
    return 0;
}
