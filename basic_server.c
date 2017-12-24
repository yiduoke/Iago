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

int main() {

  int to_client;
  int from_client;
  
  int to_client2;
  int from_client2;

  from_client = server_handshake( &to_client );
  from_client2 = server_handshake2( &to_client2 );
  
  write(to_client, "", 1);
  printf("initiated\n");
  
  int turn = 0;
  
  while(1){
    if(turn % 2 == 0){
      char move[3];
      read(from_client, move, sizeof(move));
      printf("got %s from client1\n", move);
  
      write(to_client2, move, sizeof(move));
      printf("done sending text to client2\n");
    }
    else{
      char move[1024];
      read(from_client2, move, sizeof(move));
      printf("got %s from client2\n", move);
  
      write(to_client, move, sizeof(move));
      printf("done sending text to client1\n");
    }
    turn++;
  }
}