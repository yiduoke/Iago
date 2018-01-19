#include "pipe_networking.h"

void print_chat(){
  gotoxy(0,20);
  printf("\033[0m");
  
  char chat[1024];
  char chat2[1024];

  int fd = open("chat.txt", O_RDONLY);
  read(fd, chat, sizeof(chat));
  int newlines = 0;
  char *start;
  for(; newlines < 6; newlines++){
    start = strrchr(chat, '\n');
    chat[start-chat] = 0;
  }
  printf("start-chat: %d\n", start-chat);
  lseek(fd, start-chat+1, SEEK_SET);
  read(fd, chat2, sizeof(chat2));
  printf("chat:\n[%s]\n", chat2);
}

int main(){
  print_chat();
}
