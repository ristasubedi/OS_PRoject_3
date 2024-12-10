#include "server.h"

// Use these locks and counter to synchronize
extern int numReaders;
extern pthread_mutex_t rw_lock;
extern pthread_mutex_t mutex;

extern struct node *head;
extern const char *server_MOTD;

char *trimwhitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

void *client_receive(void *ptr) {
    int client = *(int *)ptr;
    int received, i;
    char buffer[MAXBUFF];
    char cmd[MAXBUFF], username[20];
    char *arguments[80];

    // Assign guest username
    sprintf(username, "guest%d", client);
    pthread_mutex_lock(&mutex);
    head = insertFirstU(head, client, username);
    pthread_mutex_unlock(&mutex);

    send(client, server_MOTD, strlen(server_MOTD), 0);

    while (1) {
        if ((received = read(client, buffer, MAXBUFF)) > 0) {
            buffer[received] = '\0';
            strcpy(cmd, buffer);

            // Tokenize the command
            arguments[0] = strtok(cmd, delimiters);
            i = 0;
            while (arguments[i] != NULL) {
                arguments[++i] = strtok(NULL, delimiters);
            }

            // Ensure arguments are trimmed of whitespace
            for (int j = 0; j < i; j++) {
                arguments[j] = trimwhitespace(arguments[j]);
            }

            // Handle commands
            if (strcasecmp(arguments[0], "login") == 0 && i > 1) {
                pthread_mutex_lock(&mutex);
                struct node *user = findU(head, username);
                if (user != NULL) {
                    strcpy(user->username, arguments[1]);
                    sprintf(buffer, "Logged in as %s.\nchat>", arguments[1]);
                } else {
                    sprintf(buffer, "Error: Unable to login.\nchat>");
                }
                pthread_mutex_unlock(&mutex);
                send(client, buffer, strlen(buffer), 0);
            } else if (strcasecmp(arguments[0], "create") == 0 && i > 1) {
                pthread_mutex_lock(&mutex);
                head = insertFirstU(head, -1, arguments[1]); // Create a room
                pthread_mutex_unlock(&mutex);
                sprintf(buffer, "Room %s created.\nchat>", arguments[1]);
                send(client, buffer, strlen(buffer), 0);
            } else if (strcasecmp(arguments[0], "exit") == 0 || strcasecmp(arguments[0], "logout") == 0) {
                pthread_mutex_lock(&mutex);
                head = removeNodeU(head, username); // Remove user from the list
                pthread_mutex_unlock(&mutex);
                close(client);
                break;
            } else {
                sprintf(buffer, "Invalid command.\nchat>");
                send(client, buffer, strlen(buffer), 0);
            }
        }
    }
    return NULL;
}
