#include "pipe_networking.h"

void process(char *s);
void subserver(int from_client, int to_client);
void initialize();
void place_piece(int x, int y, char piece);

//2D array representation of the board initialization
char board[8][8];

int current_x = 0;
int current_y = 0;