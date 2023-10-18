#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <syscall.h>
#include <sys/wait.h>

void *system_thread_function() {
    printf("System thread pid: %d\n", getpid());
    printf("System Thread TID: %ld\n", syscall(SYS_gettid));
    system("./system_call");
    return NULL;
}

void *exec_thread_function() {
    printf("exec thread pid: %d\n", getpid());
    printf("Exec Thread TID: %ld\n", syscall(SYS_gettid));
    pid_t pid = fork();
    if (pid == 0) { 
        char *args[] = {"ls", "/usr/src", NULL};
        execvp(args[0], args);
        exit(0); 
    } else if (pid > 0) { 
        int status;
        waitpid(pid, &status, 0); 
    } else {
        perror("fork");
    }
    return NULL; 
}


int main() {
    pthread_t system_thread, exec_thread;

    printf("Main thread PID: %d\n", getpid());

    if (pthread_create(&system_thread, NULL, system_thread_function, NULL)) {
        fprintf(stderr, "Error creating system thread\n");
        return 1;
    }

    if (pthread_create(&exec_thread, NULL, exec_thread_function, NULL)) {
        fprintf(stderr, "Error creating exec thread\n");
        return 1;
    }

    pthread_join(system_thread, NULL);
    pthread_join(exec_thread, NULL); // 注意，如果exec_thread成功运行了，这里实际上不会被执行

    return 0;
}
