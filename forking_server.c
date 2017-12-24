#include "pipe_networking.h"
#include "forking_server.h"

void process(char *s);
void subserver(int from_client, int to_client, int player);

int players[1024]; // pipes for every player
int player_num = -1; // handling multiple players and matches; 0-based for easier access

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove("luigi");
    exit(0);
  }
}

void subserver(int from_client, int to_client, int player) {
  printf("[server] player count: %d\n", player_num);
  char modifying[BUFFER_SIZE];
  int to_other_client;
  if (!(player % 2)){ // player has even index
    to_other_client = players[player+1];
  }
  else{
    to_other_client = players[player-1];
  }
  printf("blocking for sending opponent move\n");
  write(to_other_client, modifying, sizeof(modifying));
  
  printf("[server] trying to read someone's move\n");
  read(from_client, modifying, sizeof(modifying));
  printf("got %s from a player\n", modifying);
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
      printf("player index: %d\n", player_num);
      to_client = server_connect(from_client);
      printf("to_client: %d\n", to_client);
      players[player_num] = to_client;

      while(1){
        if (!(player_num%2)){ // even; black
          write(to_client, "b", 1);
          write(to_client, "33w", 3);
        }
        else{ // odd, white
          write(to_client, "w", 1);
        }
	      subserver(from_client, to_client, player_num);
      }

    }
  }  
  
  return 0;
}