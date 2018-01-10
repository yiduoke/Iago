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

int main(){
  int KEY = ftok("makefile", 2); // creating a key for the .gitignore file
  int memDes;
  int* pointer;

    memDes = shmget(KEY, sizeof(int), IPC_CREAT | 0777);
    if (memDes < 0 ){//errno
      printf("failed to create shared memory; %s\n", strerror(errno));
      exit(0);
    }

    if ( (pointer = (int*)shmat(memDes, 0, 0)) < 0){
      printf("failed to attach shared memory; %s\n", strerror(errno));
      exit(0);
    }

    *pointer = 0;
    if (shmdt(pointer) < 0){
      printf("failed to detach shared memory; %s\n", strerror(errno));
      exit(0);
  }
  // printf("just created: %c in shared memory\n", *pointer);
return 0;
}
