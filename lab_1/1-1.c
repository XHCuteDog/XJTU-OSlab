#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid, pid_new;
  int flag; 
  pid = fork();
  if (pid < 0) {
    fprintf(stderr, "fork failed");
    return 1;
  } else if (!pid) {  // 子进程
    pid_new = getpid();
    flag = 1000;

    // 使用system函数调用system_call
    printf("Using system to call system_call:\n");
    system("./system_call");

    // 使用exec函数族调用system_call
    printf("Using exec to call system_call:\n");
    execlp("./system_call", "./system_call", NULL);

    // 如果execlp成功，以下代码不会执行
    printf("child: pid = %d\n", pid);
    printf("child: pid_new = %d\n", pid_new);
    printf("child: flag = %d\n", flag);
  } else {  // 父进程
    pid_new = getpid();
    flag = 2000;
    printf("parent: pid = %d\n", pid);
    printf("parent: pid_new = %d\n", pid_new);
    printf("parent: flag = %d\n", flag);
    wait(NULL);
  }
  printf("pid = %d program before end: flag = %d\n", pid, flag);
  return 0;
}
