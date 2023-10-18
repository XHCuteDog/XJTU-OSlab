#include <stdio.h>
#include <unistd.h>

int main() {
  printf("This is system_call. My PID is: %d\n", getpid());
  return 0;
}
