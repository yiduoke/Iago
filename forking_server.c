#include "pipe_networking.h"
#include "forking_server.h"

void process(char *s);
void subserver(int from_client, int to_client);

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
  
  process(modifying);
  printf("after modifying client input: %s\n", modifying);
  
  write(to_client, modifying, sizeof(modifying));
  printf("done sending modified text to client\n");
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
  
  int player_num = 0;
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
      
      while(1){
	      subserver(from_client, to_client);
      }
    }
  }  
  
  return 0;
}