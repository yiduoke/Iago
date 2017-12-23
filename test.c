#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
 
 
struct termios initial_settings,
               new_settings;
 
 
 
int main(int argc, char *argv[])
{
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
 
  while(1)
  {
    n = getchar();
 
    if(n != EOF)
    {
      key = n;
 
      if(key == 'A')  /* UP */
      {
        printf("you pressed up\n");
        // break;
      }
 
      else if(key == 'q')  /* Escape key pressed */
      {
        printf("you quit\n");
        break;
      }
    }
  }
 
  tcsetattr(0, TCSANOW, &initial_settings);
 
  return(0);
}