#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

typedef struct node {
	void *value;
	struct node *next;
} LinkedListNode;

LinkedListNode *create_node(void *v);
void prepend_node(LinkedListNode **head, LinkedListNode *new_node);
void append_node(LinkedListNode **head, LinkedListNode *new_node);
void remove_node(LinkedListNode **head, LinkedListNode *entry);

#endif