#include "pipe_networking.h"

void print_chat(){
  int lines = 0;
  // char* chat2 = (char*)calloc(1,1024);
  char chat[1024];
  char chat2[1024];

  int fd = open("chat.txt", O_RDONLY);
  read(fd, chat, sizeof(chat));

  int offset = lseek(fd, 0, SEEK_END);
  int init_offset = offset;

  for (lines; lines < 6; lines++){
    while (chat[offset--] != '\n'){
      if (!offset){ //beginning
        break;
      }
    }
    if (!offset){ //beginning
      offset = -2;
      break;
    }
  }

  lseek(fd, offset+2, SEEK_SET);
  //char* chat2 = (char*)calloc(1,1024);
  read(fd, chat2, sizeof(chat2));
  chat2[init_offset - offset-2] = 0;
  printf("%s", chat2);
}

int main(){
  print_chat();
}
