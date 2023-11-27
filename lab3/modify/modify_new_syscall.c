#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
int main()
{
    int ret = syscall(78); // after modify syscall 78
    printf("%d\n", ret);
    return 0;
}