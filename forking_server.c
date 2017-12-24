#include "pipe_networking.h"
#include <signal.h>

void subserver(int from_client, int to_client);
int player_num = -1;
int players[1024];

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove("luigi");
    exit(0);
  }
}

void subserver(int from_client, int to_client) {
  char modifying[BUFFER_SIZE];
  read(from_client, modifying, sizeof(modifying));
  printf("got %s from client\n", modifying);
  
  printf("after modifying client input: %s\n", modifying);
  
  write(to_client, modifying, sizeof(modifying));
  printf("done sending modified text to client\n");

  int i;
  for (i = 0; i<=player_num; i++){
    printf("player num's to_client pipe %d: %d\n", i, players[i]);
  }
}

// currently just his previous main he showed on the board
int main(){
  signal(SIGINT, sighandler);

  int to_client;
  int from_client;
  
  printf("server starting...\n");
  
  while(1){
    from_client = server_setup();
    printf("from_client: %d\n", from_client);
    
    int fork_id = fork();
    player_num++;

    if(!fork_id){
      printf("fork successful\n");
      to_client = server_connect(from_client);
      printf("to_client: %d\n", to_client);
      players[player_num] = to_client;

      while(1){
	      subserver(from_client, to_client);
      }
    }
  }  
  
  return 0;
}