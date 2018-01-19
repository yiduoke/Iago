#include "pipe_networking.h"

//clears terminal screen
#define clear() printf("\033[2J");
//hides cursor
#define hide_cursor() printf("\033[?25l");
//sets cursor's coords to (x,y) in terminal
#define gotoxy(x,y) printf("\033[%d;%dH", (y+1), (x+1))
//sets cursor position to the middle of the given (x,y) board tile
#define gotoBoardXY(x,y) gotoxy(4*x+2, 2*y+1)
//clears the line the cursor is on
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

// color of player piece
char color;

//pieces owned
int my_count = 2;
int enemy_count = 2;

//needed for getting input
struct termios initial_settings, new_settings;

//current cursor board coords
int current_x = 0;
int current_y = 0;

//how many possible moves you have currently
int num_legals = 0;

//used for shmem
char* pointer;
int mem_des;
int chat_size;
struct stat buffer;

//detaches and removes shmem
void clear_mem(){
  if (shmdt(pointer) < 0){
    printf("failed to detached shmem, error: %s\n", strerror(errno));
    exit(0);
  }

  if(shmctl(mem_des, IPC_RMID, 0) < 0){
    printf("failed to remove shmem, error: %s\n", strerror(errno));
  }
}

//deletes all of chat.txt
void clear_chat(){
  FILE *fp = fopen("chat.txt", "w");
  fclose(fp);
}

//handles ctrl C rage quits
static void sighandler(int signo) {
  tcsetattr(0, TCSANOW, &initial_settings);
  printf("\033[0m");
  char buffer[20];
  sprintf(buffer, "%d", getpid());
  remove(buffer);

  clear_mem();
  clear_chat();
  exit(0);
}

// change a piece of on the board
void place_piece(int x, int y, char piece){
  if (board[y][x]==' '){ //the piece is placed on an empty spot
    if (piece == color){
      my_count++;
    }
    else{
      enemy_count++;
    }
  }
  else{// the spot is not empty
    if (board[y][x] != piece){ // not a dummy move
      if (board[y][x]==color){// if that spot is gonna be turned over to the enemy
        my_count--;
        enemy_count++;
      }
      else{//the spot is turned over from the enemy into me
        my_count++;
        enemy_count--;
      }
    }
  }

  board[y][x] = piece;

  gotoBoardXY(x,y);
  if(piece == 'b') printf("\033[30m\033[42m\u2B24");
  if(piece == 'w') printf("\033[97m\033[42m\u2B24");
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

  my_count = 2;
  enemy_count = 2;

  stat("chat.txt", &buffer);
  chat_size = 0;
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
}

//checks if (x,y) is in the board
int inBounds(int x, int y){
  return x >= 0 && x < 8 && y >= 0 && y < 8;
}

//counts how many pieces would flip in a given direction if piece is placed at (x,y)
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

        // for game termination purposes
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

//converts a move into string format to send over pipes
char *string_move(int x, int y, char piece){
  char *move = (char *)calloc(3,1);
  sprintf(move, "%d%d%c", x, y, piece);
  return move;
}

//performs a move based on a string format of a move
void make_move(char *move){
  int x = move[0] - '0';
  int y = move[1] - '0';
  char piece = move[2];
  place_piece(x,y,piece);
  conquer_pieces(x,y,piece);
}

//sends move string to server
void send_move(char *move, int to_server){
  write(to_server, move, 3);
}

//highlights legal moves
void show_legals(){
  int y;
  int x;

  for(y = 0; y < 8; y++){
    for(x = 0; x < 8; x++){
      if (isLegal(x, y, color)){
        num_legals++;
	gotoBoardXY(x, y);
	printf("\033[103m \033[0m");
      }
    }
  }
}

//unhighlights legal moves
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

//creates and attaches shmem
void get_mem(){
  int KEY = ftok("makefile",0);

  mem_des = shmget(KEY, sizeof(char), 0777);
  if (mem_des < 0){
    printf("failed to get shmem, error: %s\n", strerror(errno));
    exit(0);
  }

  pointer = (char*)shmat(mem_des, NULL, 0);
  if (pointer<0){
    printf("failed to attach shmem, error: %s\n", strerror(errno));
    exit(0);
  }
}

//sets up input settings
void set_scanning(){
  tcgetattr(0,&initial_settings);

  new_settings = initial_settings;
  new_settings.c_lflag &= ~ICANON;
  new_settings.c_lflag &= ~ECHO;

  new_settings.c_cc[VMIN] = 0;
  new_settings.c_cc[VTIME] = 0;

  tcsetattr(0, TCSANOW, &new_settings);
}

//resets user input settings
void reset_scanning(){
  tcsetattr(0, TCSANOW, &initial_settings);
}

//prints chat
void print_chat(){
  gotoxy(0,20);
  printf("\033[0m");
  
  int lines = 0;
  //char* chat = (char*)calloc(1,1024);
  char chat[1024];
    
  int fd = open("chat.txt", O_RDONLY);
  int offset = lseek(fd, 0, SEEK_END);
  lseek(fd, 1, SEEK_SET); 
  read(fd, chat, sizeof(chat));
 // printf("offset: %d\n", offset);
  
  for (lines; lines < 5; lines++){
    while (chat[offset--] != '\n'){
  	}	
  }
  
  lseek(fd, offset-1, SEEK_SET);
  //char* chat2 = (char*)calloc(1,1024);
  char chat2[1024];
  read(fd, chat2, sizeof(chat2));
  printf("%s", chat2);

}

//handles user inputs
void move(int from_server, int to_server){
  char *input = (char *)calloc(1, 100);//when in doubt, calloc is always the answer
  int n;
  unsigned char key;
  set_scanning();

  // getting color from server at first
  char color_buffer[1];
  read(from_server, color_buffer, 1);
  color = color_buffer[0];

  int has_read = 0; // has read enemy move; bc we don't wanna block in every iteration, only wanna read once

  while(1){
    stat("chat.txt", &buffer);
    if(buffer.st_size > chat_size){
      print_chat();
      chat_size = buffer.st_size;
    }
    char move[3];
    if(*pointer == color && !has_read){
      gotoBoardXY(9,1);
//      printf("st_size: %d", buffer.st_size);
      read(from_server, move, 3); //receiving enemy move

      make_move(move);
      num_legals = 0;
      show_legals();

      has_read = 1;
    }

      if (!num_legals && !strncmp("33", move, 2)){ // received a dummy move bc enemy had no legal moves
        // AND I myself have no legal moves -- terminate the game
        if ((my_count + enemy_count) == 64){ // game over bc board is full
          if (my_count > enemy_count){
            printf("\033[0mYOU WON!!\n");
          }
          else{
            printf("\033[0mYOU LOST!!\n");
          }
          break;
        }
        if (!my_count){//0 pieces are your color
          printf("\033[0mYOU LOST!!\n");
          break;
        }
        if (!enemy_count){//0 pieces are enemy's color
          printf("\033[0mYOU WON!!\n");
          break;
        }
      }

    if (*pointer == color && !num_legals){// no legal moves
      // sending a dummy move
      sprintf(move, "%d%d%c", 3, 3, board[3][3]);
      send_move(move, to_server);
      has_read = 0;
    }

    n = getchar();
    if(n != EOF){
      key = n;
      if(key == 'c'){
        gotoBoardXY(0,9);
        printf("\033[0menter message:\n");
        gotoBoardXY(0,10);
        printf("\033[0m");
        char input[100];

        reset_scanning();
        fgets(input, 100, stdin);
        //printf("input: %s", input);

        FILE *fp = fopen("chat.txt", "a");
        fprintf(fp, "%s", input);
        fclose(fp);

        gotoBoardXY(0,9);
        clearLine();
        gotoBoardXY(0,10);
        clearLine();

        set_scanning();
      }
      if(key == UP){
        move_up();
        gotoBoardXY(9,0);
        clearLine();
      }
      else if(key == DOWN){
        move_down();
        gotoBoardXY(9,0);
        clearLine();
      }
      else if(key == RIGHT){
        move_right();
        gotoBoardXY(9,0);
        clearLine();
      }
      else if(key == LEFT){
        move_left();
        gotoBoardXY(9,0);
        clearLine();
      }
      else if(key == ' ' && *pointer == color){
        if(isLegal(current_x, current_y, color)){
          place_piece(current_x, current_y, color);
          conquer_pieces(current_x, current_y, color);
          hide_legals();
          gotoBoardXY(9,0);
          printf("\033[0mplaced a piece at (%d, %d)\n\033[42m", current_x, current_y);
          send_move(string_move(current_x, current_y, color), to_server);

          // moving = 0;
          has_read = 0;
        }
        else{
          gotoBoardXY(9,0);
          printf("\033[0mcan't place a piece at (%d, %d)\033[42m", current_x, current_y);
        }
      }
      else if(key == QUIT){
        gotoBoardXY(9,0);
        printf("\033[0myou rage quit\n");
        break;
      }
    }

    if ((my_count + enemy_count) == 64){ // game over bc board is full
      if (my_count > enemy_count){
        printf("\033[0mYOU WON!!\n");
      }
      else{
	      printf("\033[0mYOU LOST!!\n");
      }
      break;
    }
    if (!my_count){//0 pieces are your color
      printf("\033[0mYOU LOST!!\n");
      break;
    }
    if (!enemy_count){//0 pieces are enemy's color
      printf("\033[0mYOU WON!!\n");
      break;
    }

    gotoBoardXY(current_x, current_y);
  }

  reset_scanning();
}

int main(){
  signal(SIGINT, sighandler);

  int to_server;
  int from_server;
  get_mem();
  from_server = client_handshake( &to_server );

  clear();
  gotoxy(0,0);
  print_board();
  initialize();

  move(from_server, to_server);

  clear_mem();
}
