#include "pipe_networking.h"

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

struct termios initial_settings,
  new_settings;

int current_x = 0;
int current_y = 0;

char color;

int to_server = 0;
int from_server = 0;

//changes a piece of board[][]

void get_color(){
    char buffer[5];
    read(from_server, buffer, 1);
    color = buffer[0];
    printf("[player] my color is %c\n", color);
}

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

//handles user inputs
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
    
  to_server = 0;
  from_server = client_handshake( &to_server );
  char placed_piece[3];

  int enemy_x, enemy_y; // for receiving enemy's moves
  int move_num = 0;

  while(1){
    get_color();
    // receiving enemy's move
    printf("blocking when receiving enemy move\n");
    read(from_server, placed_piece, sizeof(placed_piece));
    enemy_x = placed_piece[0] - '0';
    enemy_y = placed_piece[1] - '0';
    place_piece(enemy_x, enemy_y, placed_piece[2]);

    n = getchar();
    printf("is getchar run?\n");
    if(n != EOF){
      printf("[black] is this running? after EOF");
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
      else if(key == B){
	    if(isLegal(current_x, current_y, 'b')){
            move_num++;

	        place_piece(current_x, current_y, 'b');
	        conquer_pieces(current_x, current_y, 'b');
	        gotoBoardXY(0,9);
            printf("\033[0mplaced a black piece at (%d, %d)\033[42m", current_x, current_y);
      
            placed_piece[0] = current_x + '0';
            placed_piece[1] = current_y + '0';
            placed_piece[2] = 'b';

            printf("writing to server about the move now\n");
            write(to_server, placed_piece, sizeof(placed_piece));
	    }
	    else{
	        gotoBoardXY(0,9);
	        printf("\033[0mcan't place a black piece at (%d, %d)\033[42m", current_x, current_y);
	    }
      }
      else if(key == W){
	    if(isLegal(current_x, current_y, 'w')){
            move_num++;

	        place_piece(current_x, current_y, 'w');
	        conquer_pieces(current_x, current_y, 'w');
	        gotoBoardXY(0, 9);
            printf("\033[0mplaced a white piece at (%d, %d)\033[42m", current_x, current_y);
      
            placed_piece[0] = current_x + '0';
            placed_piece[1] = current_y + '0';
            placed_piece[2] = 'w'; 

            printf("writing to server about the move now\n");
            write(to_server, placed_piece, sizeof(placed_piece));
	    }
	    else{
	        gotoBoardXY(0,9);
	        printf("\033[0mcan't place a white piece at (%d, %d)\033[42m", current_x, current_y);
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

static void sighandler(int signo) {
    if (signo == SIGINT) {
      char buffer[HANDSHAKE_BUFFER_SIZE];
      sprintf(buffer, "%d", getpid());
      remove(buffer); // removes the private pipe named after the pid; use this in move()
      exit(0);
    }
}

int main(){
    clear();
    gotoxy(0,0);
    print_board();
    initialize();
  
    move();
}
