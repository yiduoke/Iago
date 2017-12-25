#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client
  Perofrms the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  remove("public");
  mkfifo("public", 0777);
  int upstream = open("public", O_RDONLY);
  
  char input[256];
  read(upstream, input, sizeof(input));
  printf("input: %s\n", input);
  remove("public");
  
  int downstream = open(input, O_WRONLY);
  write(downstream, "received", 9);
  
  read(upstream, input, sizeof(input));
  printf("final: %s\n", input);
  
  *to_client = downstream;
  
  return upstream;
}

int server_handshake2(int *to_client) {
  remove("public2");
  mkfifo("public2", 0777);
  int upstream = open("public2", O_RDONLY);
  
  char input[256];
  read(upstream, input, sizeof(input));
  printf("input: %s\n", input);
  remove("public2");
  
  int downstream = open(input, O_WRONLY);
  write(downstream, "received", 9);
  
  read(upstream, input, sizeof(input));
  printf("final: %s\n", input);
  
  *to_client = downstream;
  
  return upstream;
}

/*=========================
  client_handshake
  args: int * to_server
  Perofrms the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  char name[100];
  sprintf(name, "%d", (int)getpid());
  printf("pid: %s\n",name);
  remove(name);
  mkfifo(name, 0777);
  
  if( access( "public", F_OK ) != -1 ) {
    // file exists
    int upstream = open("public", O_WRONLY);
    write(upstream, name, sizeof(name));
  
    int downstream = open(name, O_RDONLY);
    char input[256];
    read(downstream, input, sizeof(input));
    remove(name);
    printf("input: %s\n", input);
  
    write(upstream, "handhshook", 11);
  
    *to_server = upstream;

    return downstream;
  } else {
    // file doesn't exist
    int upstream = open("public2", O_WRONLY);
    write(upstream, name, sizeof(name));
  
    int downstream = open(name, O_RDONLY);
    char input[256];
    read(downstream, input, sizeof(input));
    remove(name);
    printf("input: %s\n", input);
  
    write(upstream, "handhshook", 11);
  
    *to_server = upstream;

    return downstream;
  }
}
