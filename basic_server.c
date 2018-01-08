#include "pipe_networking.h"
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

void create_mem(){
  int KEY = ftok("makefile",11);
  int* pointer;
  int mem_des;

  mem_des = shmget(KEY, sizeof(int), IPC_CREAT | IPC_EXCL | 0777);
  if (mem_des < 0){
    printf("failed to create shared memory, error is %s\n", strerror(errno));
    exit(0);
  }
  // gotoBoardXY(0,11);
  // printf("shmem created\n");
  //
  // pointer = (int*)shmat(mem_des,0,0);
  // if (pointer<0){
  //   printf("failed to attach shared memory, error is %s\n", strerror(errno));
  //   exit(0);
  // }

  // *pointer = 'b';
  // if (shmdt(pointer) < 0){
  //   printf("failed to detached shared memory, error is %s\n", strerror(errno));
  //   exit(0);
  // }
  // printf("just created: %c in shared memory\n", *pointer);
}

int main() {

  int to_client;
  int from_client;

  int to_client2;
  int from_client2;

  from_client = server_handshake( &to_client );
  from_client2 = server_handshake2( &to_client2 );

  write(to_client, "33w", 3);
  printf("initiated\n");

  int turn = 0;

  while(1){
    if(turn % 2 == 0){
      char move[3];
      read(from_client, move, sizeof(move));
      printf("got move from client1\n");

      write(to_client2, move, sizeof(move));
      printf("done sending move to client2\n");
    }
    else{
      char move[3];
      read(from_client2, move, sizeof(move));
      printf("got move from client2\n");

      write(to_client, move, sizeof(move));
      printf("done sending move to client1\n");
    }
    turn++;
  }
}
