#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <termios.h>

#ifndef NETWORKING_H
#define NETWORKING_H

#define ACK "HOLA"
#define HANDSHAKE_BUFFER_SIZE 20
#define BUFFER_SIZE 1000

int server_handshake(int *to_client);
int client_handshake(int *to_server);










int server_setup();
int server_connect(int from_client);

#endif
