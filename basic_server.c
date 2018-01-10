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
#include <stdlib.h>

static void sighandler(int signo) {
  int KEY = ftok("makefile", 11);
  int mem_des = shmget(KEY, sizeof(int), 0777);

  if(shmctl(mem_des, IPC_RMID, 0) < 0){
    printf("failed to remove shared memory\n");
  }
  exit(0);
}

char *pointer;

void create_mem(){
  int KEY = ftok("makefile",11);
  int mem_des = shmget(KEY, sizeof(char), IPC_CREAT | 0777);
  //pointer = (char*)calloc(1, sizeof(char*));

  if (mem_des < 0){
    printf("failed to create shared memory, error is %s\n", strerror(errno));
    exit(0);
  }

  // pointer = (char*)shmat(mem_des,NULL,0);
  pointer = shmat(mem_des, 0, 0);
  if (pointer<0){
    printf("failed to attach shared memory, error is %s\n", strerror(errno));
    exit(0);
  }

  *pointer = 'b';

  // printf("pointer: %c\n", *pointer);

  // if (shmdt(pointer) < 0){
  //   printf("failed to detached shared memory, error is %s\n", strerror(errno));
  //   exit(0);
  // }
}

int main() {
  signal(SIGINT, sighandler);
  create_mem();
  // *pointer = 'd';
  //printf("pointer: %c\n", *pointer);
  int to_client;
  int from_client;

  int to_client2;
  int from_client2;

  from_client = server_handshake( &to_client );
  from_client2 = server_handshake2( &to_client2 );

  write(to_client, "b", 1);
  write(to_client2, "w", 1);

  write(to_client, "33w", 3);
  printf("initiated\n");

  // int turn = 0;

  while(1){
    // if(turn % 2 == 0){
    char move[3];
    if(*pointer == 'b'){
      read(from_client, move, sizeof(move));
      printf("got move from client1\n");

      write(to_client2, move, sizeof(move));
      printf("done sending move to client2\n");
      *pointer = 'w';
    }
    else{
      // char move[3];
      read(from_client2, move, sizeof(move));
      printf("got move from client2\n");

      write(to_client, move, sizeof(move));
      printf("done sending move to client1\n");
      *pointer = 'b';
    }
    printf("current turn: %c\n", *pointer);
    // turn++;
  }
}
