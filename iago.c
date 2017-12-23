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

#define clear() printf("\033[2J");
#define hide_cursor() printf("\033[?25l");
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))
#define UP "w"
#define DOWN "S"
#define RIGHT "^[[C"
#define LEFT "^[[D"

  void print_board(){
    int file = open("board.txt", O_RDONLY);
    char buffer[1024];
    read(file, buffer, sizeof(buffer));
    printf("%s", buffer);
}

void move(){
    char *input = (char *)calloc(1, 1024);//when in doubt, calloc is always the answer
    // fgets(input, 2, stdin);
    scanf("%1s", input);
    printf("input: [%s]\n", input);
    // if(!strncmp(UP, input, 1)) printf("up pressed\n");
}
int main(){
    hide_cursor();
    clear();
    // while(1){
        gotoxy(0,0);
        print_board();
        move();
    // }
}