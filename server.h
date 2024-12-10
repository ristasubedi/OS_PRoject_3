#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <netdb.h>
#include <ctype.h>
#include <pthread.h>
#include "list.h"
#include <strings.h> // For strcasecmp

// Constants
#define MAX_READERS 25
#define TRUE   1  
#define FALSE  0  
#define PORT 8888  
#define delimiters " "
#define max_clients  30
#define DEFAULT_ROOM "Lobby"
#define MAXBUFF   2096
#define BACKLOG 2 

// Shared variables
extern int numReaders;                           // Reader count
extern pthread_mutex_t mutex;                    // Mutex lock
extern pthread_mutex_t rw_lock;                  // Read/Write lock
extern struct node *head;                        // Linked list head for users and rooms
extern const char *server_MOTD;                  // Message of the day

// Function prototypes
int get_server_socket();
int start_server(int serv_socket, int backlog);
int accept_client(int serv_sock);
void sigintHandler(int sig_num);
void *client_receive(void *ptr);

#endif
