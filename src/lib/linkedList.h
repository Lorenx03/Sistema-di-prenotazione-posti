#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

struct node {
    int value;
    struct node *next;
};

struct node *create_node(int v);
void prepend_node(struct node **head, struct node *new_node);
void append_node(struct node **head, struct node *new_node);
void remove_node(struct node **head, struct node *entry);

#endif