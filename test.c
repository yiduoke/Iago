// #include <stdlib.h>
// #include <stdio.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/wait.h>
// #include <sys/types.h>
// #include <sys/ipc.h>
// #include <sys/sem.h>
// #include <sys/shm.h>
// #include <errno.h>
// #include <termios.h>
//
// int main(){
//   int KEY = ftok("makefile",1);
//   int mem_des;
//   int* pointer;
//
//   printf("KEY: %d\n", KEY);
//   mem_des = shmget(KEY, sizeof(int), IPC_CREAT | 0777);
//   if (mem_des < 0){
//     printf("failed to create shared memory, error is %s\n", strerror(errno));
//     exit(0);
//   }
//
//   pointer = (int*)shmat(mem_des,NULL,0);
//   if (pointer<0){
//     printf("failed to attach shared memory, error is %s\n", strerror(errno));
//     exit(0);
//   }
//
//   *pointer = 'b';
//   if (shmdt(pointer) < 0){
//     printf("failed to detached shared memory, error is %s\n", strerror(errno));
//     exit(0);
//   }
//   printf("just created: %c in shared memory\n", *pointer);
// }

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>

#define KEY 24601

int main(){
  int f;
  char *var;

  int shmd = shmget(KEY, sizeof(char), IPC_CREAT | 0600);

  var = shmat(shmd, 0, 0);

  *var = 'B';

  printf("value of var: %c \n", *var);

  shmdt( var);
  shmctl(shmd, IPC_RMID, 0);
}
