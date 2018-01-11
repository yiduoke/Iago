#include "pipe_networking.h"

//shmem to keep track of whose turn it is
int mem_des;
char *pointer;

//detaches and removes shmem
void clear_shmem(){
  if (shmdt(pointer) < 0){
    printf("failed to detached shmem, error: %s\n", strerror(errno));
    exit(0);
  }

  if(shmctl(mem_des, IPC_RMID, 0) < 0){
    printf("failed to remove shmem, error: %s\n", strerror(errno));
  }
}

//handles ctrl C rage quits
static void sighandler(int signo) {
  clear_shmem();
  exit(0);
}

//creates shmem
void create_shmem(){
  int KEY = ftok("makefile",0);
  mem_des = shmget(KEY, sizeof(char), IPC_CREAT | 0777);
  if (mem_des < 0){
    printf("failed to create shmem, error: %s\n", strerror(errno));
    exit(0);
  }

  pointer = shmat(mem_des, 0, 0);
  if (pointer<0){
    printf("failed to attach shmem, error: %s\n", strerror(errno));
    exit(0);
  }

  *pointer = 'b';
}

int main() {
  signal(SIGINT, sighandler);
  create_shmem();

  int from_b;
  int to_b;
  int from_w;
  int to_w;
  char move[3];

  from_b = server_handshake(&to_b);
  from_w = server_handshake2(&to_w);

  write(to_b, "b", 1);
  write(to_w, "w", 1);

  write(to_b, "33w", 3);
  printf("game started\n");

  while(1){
    if(*pointer == 'b'){
      read(from_b, move, sizeof(move));
      printf("got move from b\n");

      write(to_w, move, sizeof(move));
      printf("done sending move to w\n");
      *pointer = 'w';
    }
    else{
      read(from_w, move, sizeof(move));
      printf("got move from w\n");

      write(to_b, move, sizeof(move));
      printf("done sending move to b\n");
      *pointer = 'b';
    }
    printf("current turn: %c\n", *pointer);
  }

  printf("game finished\n");
  clear_shmem();
}
