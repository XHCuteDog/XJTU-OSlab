#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void signal_handler(int signal_num) {
  if (signal_num == SIGALRM) {
    printf("the signal is timeout proc stopped auto!!\n");
  }
  // printf("Received interrupt signal: %d\n", signal_num);
}

void child_signal_handler(int signal_num) {
  if (signal_num == 16) {
    printf("Child process 1 is killed by parent !!\n");
  }
  if (signal_num == 17) {
    printf("Child process 2 is killed by parent !!\n");
  }
  exit(0);
}

int main() {
  pid_t child1, child2;
  signal(SIGINT, signal_handler);   //  (Ctrl+C)
  signal(SIGQUIT, signal_handler);  // (Ctrl+\)
  signal(SIGALRM, signal_handler);

  child1 = fork();
  if (child1 == 0) {
    signal(16, child_signal_handler);
    while (1) {
    }
  }

  child2 = fork();
  if (child2 == 0) {
    signal(17, child_signal_handler);
    while (1) {
    }
  }

  if (child1 > 0 && child2 > 0) {
    alarm(5);
    pause();

    kill(child1, 16);
    kill(child2, 17);

    // wait two child process to exit
    wait(NULL);
    wait(NULL);
    printf("Parent process is killed!!\n");
  }

  return 0;
}

