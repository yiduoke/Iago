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

#define clear() printf("\033[2J");
#define hide_cursor() printf("\033[?25l");
#define gotoxy(x,y) printf("\033[%d;%dH", (y+1), (x+1))
//sets cursor position to the middle of the given (x,y) tile
#define gotoBoardXY(x,y) gotoxy(4*x+2, 2*y+1)
#define clearLine() printf("\033[0m\033[K\033[42m");

#define UP 'A'
#define DOWN 'B'
#define RIGHT 'C'
#define LEFT 'D'
#define QUIT 'q'
#define B 'b'
#define W 'w'

//2D array representation of the board initialization
char board[8][8];

//
char color;

struct termios initial_settings,
  new_settings;

int current_x = 0;
int current_y = 0;

//changes a piece of board[][]
void place_piece(int x, int y, char piece){
  board[y][x] = piece;
  gotoBoardXY(x,y);
  if(piece == 'b') printf("\033[30mb");
  if(piece == 'w') printf("\033[97mw");
}

//sets board[][] to all spaces, then adds initial pieces
void initialize(){
  int y;
  int x;
  for(y = 0; y < 8; y++){
    for(x = 0; x < 8; x++){
      board[y][x] = ' ';
    }
  }

  place_piece(3,3,'w');
  place_piece(3,4,'b');
  place_piece(4,3,'b');
  place_piece(4,4,'w');
}

//prints empty board
void print_board(){
  printf("\033[30m");
  printf("\033[42m");
  int file = open("board.txt", O_RDONLY);
  char buffer[1024];
  read(file, buffer, sizeof(buffer));
  close(file);
  printf("%s", buffer);

  //printf("\033[0m");
}

//prints current configuration of board[][]
void update_board(){
  int y;
  int x;
  for(y = 0; y < 8; y++){
    for(x = 0; x < 8; x++){
      gotoBoardXY(x, y);
      if(board[y][x] == 'b') printf("\033[1;30m");
      //if(board[y][x] == ' ') printf("\033[1;42m");
	
      printf("%c", board[y][x]);
    }
  }
}

//checks if (x,y) is in the board
int inBounds(int x, int y){
  return x >= 0 && x < 8 && y >= 0 && y < 8;
}

//counts how many pieces would flip in a given direction if piece is places at (x,y)
int conquer_count(int x, int y, int xDir, int yDir, char piece){
  int count = 0;
  x += xDir;
  y += yDir;

  char otherPiece = 'b';
  if(piece == 'b') otherPiece = 'w';
  
  while(inBounds(x,y) && board[y][x] == otherPiece){
    x += xDir;
    y += yDir;
    count++;
  }

  if(!inBounds(x,y) || board[y][x] != piece) count = 0;
  return count;
}

//checks if it's legal to place piece at (x,y)
int isLegal(int x, int y, char piece){
  if(board[y][x] != ' ') return 0;

  int xDir;
  int yDir;
  
  for(yDir = -1; yDir < 2; yDir++){
    for(xDir = -1; xDir < 2; xDir++){
      if(xDir == 0 && yDir == 0) continue;
      
      if(conquer_count(x,y,xDir,yDir, piece)) return 1;
    }
  }

  return 0;
}

//flips all pieces that should be flipped if piece is placed at (x,y)
void conquer_pieces(int x, int y, char piece){
  int xDir;
  int yDir;
  
  for(yDir = -1; yDir < 2; yDir++){
    for(xDir = -1; xDir < 2; xDir++){
      if(xDir == 0 && yDir == 0) continue;
      
      int count = conquer_count(x,y,xDir,yDir, piece);
      int newX = x;
      int newY = y;
      
      while(count){
	newX += xDir;
	newY += yDir;
	place_piece(newX, newY, piece);
	count--;
      }
    }
  }
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

char *string_move(int x, int y, char piece){
  char *move = (char *)calloc(3,1);
  sprintf(move, "%d%d%c", x, y, piece);
  return move;
}

void make_move(char *move){
  int x = move[0] - '0';
  int y = move[1] - '0';
  char piece = move[2];
  place_piece(x,y,piece);
  conquer_pieces(x,y,piece);
}

void send_move(char *move, int to_server){
  write(to_server, move, 3);
}

void show_legals(){
  int y;
  int x;
  
  for(y = 0; y < 8; y++){
    for(x = 0; x < 8; x++){
      if (isLegal(x, y, color)){
	gotoBoardXY(x, y);
	printf("\033[103m \033[0m");
      }
    }
  }
}

void hide_legals(){
  int y;
  int x;
  
  for(y = 0; y < 8; y++){
    for(x = 0; x < 8; x++){
      if (board[y][x] == ' '){
	gotoBoardXY(x, y);
	printf("\033[42m ");
      }
    }
  }
}

//handles user inputs
void move(int from_server, int to_server){  
  char *input = (char *)calloc(1, 100);//when in doubt, calloc is always the answer
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
  
  int moving = 0;
  while(1){
    if(!moving){
      char move[3];
      read(from_server, move, 3);
      
      color = 'b';
      if(move[2] == 'b') color = 'w';
      
      gotoBoardXY(0,9);
      make_move(move);
      show_legals();
      moving = 1;
    }
    
    n = getchar();
    if(n != EOF){
      key = n;
      if(key == UP){
	move_up();
	gotoBoardXY(0,9);
	clearLine();
      }
      else if(key == DOWN){
	move_down();
	gotoBoardXY(0,9);
	clearLine();
      }
      else if(key == RIGHT){
	move_right();
	gotoBoardXY(0,9);
	clearLine();
      }
      else if(key == LEFT){
	move_left();
	gotoBoardXY(0,9);
	clearLine();
      }
      else if(key == ' '){
	if(isLegal(current_x, current_y, color)){
	  place_piece(current_x, current_y, color);
	  conquer_pieces(current_x, current_y, color);
	  hide_legals();
	  gotoBoardXY(0,9);
	  printf("\033[0mplaced a piece at (%d, %d)\n\033[42m", current_x, current_y);
	  send_move(string_move(current_x, current_y, color), to_server);
	  moving = 0;
	}
	else{
	  gotoBoardXY(0,9);
	  printf("\033[0mcan't place a piece at (%d, %d)\033[42m", current_x, current_y);
	}
      }
      else if(key == QUIT){
	printf("\033[0myou rage quit\n");
	break;
      }
    }

    gotoBoardXY(current_x, current_y);
  }
  tcsetattr(0, TCSANOW, &initial_settings);
}

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

int main(){
  int to_server;
  int from_server;
  
  from_server = client_handshake( &to_server );
  
  clear();
  gotoxy(0,0);
  print_board();
  initialize();
  
  
  move(from_server, to_server);
}
