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

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );

  while(1){
    char modified[1024];
    read(from_server, modified, sizeof(modified));
    printf("received %s\n", modified);
    
    char input[1024];
    printf("please type something: ");
    fgets(input, 1000, stdin); 
  
    printf("user input was %s\n", input);
  
    write(to_server, input, sizeof(input));
  }
}
