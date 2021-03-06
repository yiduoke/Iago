#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <termios.h>

#ifndef NETWORKING_H
#define NETWORKING_H
#define ACK "HOLA"

#define HANDSHAKE_BUFFER_SIZE 10
#define BUFFER_SIZE 1000

int server_handshake(int *to_client);

int server_handshake2(int *to_client);

int client_handshake(int *to_server);

#endif
