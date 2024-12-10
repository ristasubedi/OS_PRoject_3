#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"

struct node* insertFirstU(struct node *head, int socket, char *username) {
    struct node *link = (struct node *)malloc(sizeof(struct node));

    if (link == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    link->socket = socket;
    strcpy(link->username, username);

    link->next = head; // Point it to the old first node
    return link;       // Return the new head
}

struct node* findU(struct node *head, char* username) {
    struct node *current = head;

    while (current != NULL && strcmp(current->username, username) != 0) {
        current = current->next; // Go to next link
    }

    return current; // Returns NULL if not found
}

struct node* removeNodeU(struct node *head, char *username) {
    struct node *current = head, *previous = NULL;

    while (current != NULL && strcmp(current->username, username) != 0) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        return head; // Node not found
    }

    if (previous == NULL) {
        head = current->next; // Remove the first node
    } else {
        previous->next = current->next; // Bypass the current node
    }

    free(current); // Free the removed node
    return head;
}
