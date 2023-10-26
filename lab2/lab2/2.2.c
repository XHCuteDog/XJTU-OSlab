#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int pid1, pid2;

int main() {
  int fd[2];
  char InPipe[1000];
  char c1 = '1', c2 = '2';
  pipe(fd);

  while ((pid1 = fork()) == -1)
    ;  // ������� 1 �������ɹ�,���ѭ��
       // ����ӽ��� 1 �����ɹ�,pid1 Ϊ���̺�
  if (pid1 == 0) {
    lockf(fd[1], 1, 0);  // �����ܵ� 0 para means lock until the end
    for (int i = 0; i < 2000; i++) {
      write(fd[1], &c1, 1);
    }                    // ��2000 ��ÿ����ܵ�д���ַ���1��
    sleep(5);            // �ȴ������̶�������
    lockf(fd[1], 0, 0);  // ����ܵ�������
    exit(0);             // �������� 1
  } else {
    while ((pid2 = fork()) == -1)
      ;  // ������ 2 �������ɹ�,���ѭ��
    if (pid2 == 0) {
      lockf(fd[1], 1, 0);
      for (int i = 0; i < 2000; i++) {
        write(fd[1], &c2, 1);
      }  // ��2000 ��ÿ����ܵ�д���ַ���2��
      sleep(5);
      lockf(fd[1], 0, 0);
      exit(0);
    } else {
      wait(NULL);                 // �ȴ��ӽ��� 1 ����
      wait(0);                    // �ȴ��ӽ��� 2 ����
      read(fd[0], InPipe, 4000);  // �ӹܵ��ж��� 4000 ���ַ�
      InPipe[4000] = '\0';        // ���ַ���������
      printf("%s\n", InPipe);     // ��ʾ����������
      exit(0);                    // �����̽���
    }
  }
}
