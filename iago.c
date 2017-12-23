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
#include <termios.h>

#define clear() printf("\033[2J");
#define hide_cursor() printf("\033[?25l");
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))
#define UP 'A'
#define DOWN 'B'
#define RIGHT 'C'
#define LEFT 'D'
#define QUIT 'q'

struct termios initial_settings,
new_settings;

int current_x = 0;
int current_y = 0;

//2D array representation of the board initialization
char board[8][8]; 
void initialize(){
  for(int r = 0; r < 8; r++){
    for(int c = 0; c < 8; c++){
      board[r][c] = ' ';
    }
  }

  board[3][3] = 'w';
  board[3][4] = 'b';
  board[4][3] = 'w';
  board[4][4] = 'b';
}

void print_board(){
    int file = open("board.txt", O_RDONLY);
    char buffer[1024];
    read(file, buffer, sizeof(buffer));
    printf("%s", buffer);
}

void move_up(){
    if (current_y - 1 > - 1){// not out of bounds
        current_y--;
    }
}

void move_down(){
    if (current_y + 1 < 8){// not out of bounds
        current_y++;
    }
}

void move_right(){
    if (current_x + 1 < 8){// not out of bounds
        current_x++;
    }
}

void move_left(){
    if (current_x - 1 > - 1){// not out of bounds
        current_x--;
    }
}

void move(){
    char *input = (char *)calloc(1, 1024);//when in doubt, calloc is always the answer
    int n; 
    unsigned char key;

    tcgetattr(0,&initial_settings);
    
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 0;
    new_settings.c_cc[VTIME] = 0;
    
    tcsetattr(0, TCSANOW, &new_settings);
    
    while(1){
        n = getchar();
        if(n != EOF){
            key = n;
            if(key == UP){
                printf("you pressed up\n");
                move_up();
            }
            else if(key == DOWN){
                printf("you pressed down\n");
                move_down();
            }
            else if(key == RIGHT){
                printf("you pressed right\n");
                move_right();
            }
            else if(key == LEFT){
                printf("you pressed left\n");
                move_left();
            }
            else if(key == QUIT){
                printf("you rage quit\n");
                break;
            }
        }
        printf("current x: %d, current y: %d", current_x, current_y);
    }
    tcsetattr(0, TCSANOW, &initial_settings);
}

int main(){
    initialize();
    hide_cursor();
    clear();
    gotoxy(0,0);
    print_board();
    move();
}