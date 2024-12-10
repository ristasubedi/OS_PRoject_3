#ifndef LIST_H
#define LIST_H

struct node {
    char username[30];
    int socket;
    struct node *next;
};

struct node* insertFirstU(struct node *head, int socket, char *username);
struct node* findU(struct node *head, char* username);
struct node* removeNodeU(struct node *head, char *username);

#endif
