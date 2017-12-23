#include "pipe_networking.h"
#include "forking_server.h"

#define clear() printf("\033[2J");
#define hide_cursor() printf("\033[?25l");
#define gotoxy(x,y) printf("\033[%d;%dH", (y+1), (x+1))
//sets cursor position to the middle of the given (x,y) tile
#define gotoBoardXY(x,y) gotoxy(4*x+2, 2*y+1)

#define UP 'A'
#define DOWN 'B'
#define RIGHT 'C'
#define LEFT 'D'
#define QUIT 'q'
#define SPACE ' '

//2D array representation of the board initialization

struct termios initial_settings,
new_settings;

//sets board[][] to all spaces, then adds initial pieces
// void initialize(){
//   int y;
//   int x;
//   for(y = 0; y < 8; y++){
//     for(x = 0; x < 8; x++){
//       board[y][x] = ' ';
//     }
//   }

//   board[3][3] = 'w';
//   board[3][4] = 'b';
//   board[4][3] = 'b';
//   board[4][4] = 'w';
// }

//prints empty board
void print_board(){
    int file = open("board.txt", O_RDONLY);
    char buffer[1024];
    read(file, buffer, sizeof(buffer));
    printf("%s", buffer);
}

//prints current configuration of board[][]
void update_board(){
  int y;
  int x;
  for(y = 0; y < 8; y++){
    for(x = 0; x < 8; x++){
      gotoBoardXY(x, y);
      printf("%c", board[y][x]);
    }
  }
}

//changes a piece of board[][]
// void place_piece(int x, int y, char piece){
//   board[y][x] = piece;
// }

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

    // handshake the server
    int to_server = 0;
    int from_server = 0;
    char placed_piece[3];
  
    from_server = client_handshake( &to_server );
    
    while(1){
        n = getchar();
        if(n != EOF){
            key = n;
            if(key == UP){
                move_up();
            }
            else if(key == DOWN){
                move_down();
            }
            else if(key == RIGHT){
                move_right();
            }
            else if(key == LEFT){
                move_left();
            }
	    else if(key == SPACE){
        //   place_piece(current_x, current_y, 'b');
            placed_piece[0] = current_x + '0';
            placed_piece[1] = current_y + '0';
            placed_piece[2] = 'b'; // let's make it black for now

            write(to_server, placed_piece, sizeof(placed_piece));
            read(from_server, placed_piece, sizeof(placed_piece));
            update_board();
        }
        else if(key == QUIT){
            printf("you rage quit\n");
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
      remove(buffer);
      exit(0);
    }
}

int main(){
    signal(SIGINT, sighandler);
    // initialize();
    clear();
    gotoxy(0,0);
    print_board();
    update_board();
    move();

}
