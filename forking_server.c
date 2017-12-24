#include "pipe_networking.h"
#include "forking_server.h"

void process(char *s);
void subserver(int from_client, int to_client);

int players[1024]; // pipes for every player

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove("luigi");
    exit(0);
  }
}

void subserver(int from_client, int to_client, int player) {
  char modifying[BUFFER_SIZE];
  read(from_client, modifying, sizeof(modifying));
  printf("got %s from client\n", modifying);
  
  process(modifying);
  printf("after modifying client input: %s\n", modifying);
  
  write(to_client, modifying, sizeof(modifying));

  int to_other_client;
  if (!(player % 2)){ // player has even index
    to_other_client = players[player+1];
  }
  else{
    to_other_client = players[player-1];
  }
  write(to_other_client, modifying, sizeof(modifying));
  // read(from_other_client, modifying, sizeof(modify));
}

void process(char * s) {
  int data[2];
  data[0] = s[0] - '0';
  data[1] = s[1] - '0';
  
  s[0] = data[0];
  s[1] = data[1];

  printf("1st number: %d\n", data[0]);
  printf("2nd number: %d\n", data[1]);
  printf("color sent from the player: %c\n", s[2]);
}

// currently just his previous main he showed on the board
int main(){
  signal(SIGINT, sighandler);

  int to_client;
  int from_client;
  
  printf("server starting...\n");
  
  int player_num = -1; // handling multiple players and matches; 0-based for easier access
  while(1){
    from_client = server_setup();
    printf("from_client: %d\n", from_client);
    
    int fork_id = fork();
    player_num++;

    if(!fork_id){
      printf("fork successful\n");
      printf("player count: %d\n", player_num);
      to_client = server_connect(from_client);
      printf("to_client: %d\n", to_client);
      players[player_num] = to_client;

      while(1){
	      subserver(from_client, to_client, player_num);
      }

    }
  }  
  
  return 0;
}