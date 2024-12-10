// Group Members: 
// Rista Subedi
// Divya Shrestha
// Suyog Bala
// Manushi Khatri
#include "server.h"

int chat_serv_sock_fd; // server socket

// Shared variables
int numReaders = 0;                           // Reader count
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex lock
pthread_mutex_t rw_lock = PTHREAD_MUTEX_INITIALIZER; // Read/Write lock
struct node *head = NULL;                     // Linked list head for users and rooms
const char *server_MOTD = "Thanks for connecting to the BisonChat Server.\n\nchat>";

int main(int argc, char **argv) {
    signal(SIGINT, sigintHandler);

    // Create the default room
    pthread_mutex_lock(&mutex);
    head = insertFirstU(head, -1, DEFAULT_ROOM); // -1 indicates it's a room, not a user
    pthread_mutex_unlock(&mutex);

    // Open server socket
    chat_serv_sock_fd = get_server_socket();

    // Start the server
    if (start_server(chat_serv_sock_fd, BACKLOG) == -1) {
        perror("start_server error");
        exit(EXIT_FAILURE);
    }

    printf("Server Launched! Listening on PORT: %d\n", PORT);

    while (1) {
        int new_client = accept_client(chat_serv_sock_fd);
        if (new_client != -1) {
            pthread_t new_client_thread;
            pthread_create(&new_client_thread, NULL, client_receive, (void *)&new_client);
        }
    }

    close(chat_serv_sock_fd);
    return 0;
}

int get_server_socket() {
    int opt = TRUE;   
    int master_socket;
    struct sockaddr_in address; 
    
    // Create a master socket  
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {   
        perror("Socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
    // Set master socket to allow multiple connections
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {   
        perror("Setsockopt");   
        exit(EXIT_FAILURE);   
    }   
     
    // Set address and port
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons(PORT);   
         
    // Bind the socket to localhost port 8888  
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {   
        perror("Bind failed");   
        exit(EXIT_FAILURE);   
    }   

    return master_socket;
}

int start_server(int serv_socket, int backlog) {
    int status = 0;
    if ((status = listen(serv_socket, backlog)) == -1) {
        perror("Socket listen error");
    }
    return status;
}

int accept_client(int serv_sock) {
    int reply_sock_fd = -1;
    socklen_t sin_size = sizeof(struct sockaddr_storage);
    struct sockaddr_storage client_addr;

    // Accept a connection request from a client
    if ((reply_sock_fd = accept(serv_sock, (struct sockaddr *)&client_addr, &sin_size)) == -1) {
        perror("Socket accept error");
    }
    return reply_sock_fd;
}

void sigintHandler(int sig_num) {
    printf("\nShutting down the server...\n");

    pthread_mutex_lock(&mutex);
    struct node *temp = head;
    while (temp != NULL) {
        if (temp->socket != -1) { // Close only user sockets
            close(temp->socket);
        }
        temp = temp->next;
    }
    pthread_mutex_unlock(&mutex);

    printf("All user connections closed. Cleaning up resources...\n");
    while (head != NULL) {
        struct node *to_free = head;
        head = head->next;
        free(to_free);
    }

    close(chat_serv_sock_fd);
    exit(0);
}
