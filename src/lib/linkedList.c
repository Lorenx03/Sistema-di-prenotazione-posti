#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>


struct node {
	int value;
	struct node *next;
};


struct node *create_node(int v){
	struct node *new_node = malloc(sizeof(struct node));

	if(!new_node){
		fprintf(stderr, "Error in memory allocation");
		exit(EXIT_FAILURE);
	}

	new_node->value = v;
	new_node->next = NULL;

	return new_node;
}



void prepend_node(struct node **head, struct node *new_node){
	new_node->next = *head;
	*head = new_node;
}



void append_node(struct node **head, struct node *new_node){
	if(*head == NULL){
		*head = new_node;
	}else{
		struct node *currentNode = *head;
		while(currentNode->next){
			currentNode = currentNode->next;
		}

		currentNode->next = new_node;
	}
}



void remove_node(struct node **head, struct node *entry){
	while((*head) != entry){
		head = &(*head)->next;
	}
	*head = entry->next;

	free(entry);
}



// void print_list(struct node *head){
// 	while(head){
// 		printf("%d \n", head->value);
// 		head = head->next;
// 	}
// }


// int main(){
// 	struct node *head = NULL;
// 	struct node *nodeToElimin = create_node(2);
// 	append_node(&head, nodeToElimin);
// 	append_node(&head, create_node(3));
// 	append_node(&head, create_node(4));
// 	append_node(&head, create_node(5));
// 	append_node(&head, create_node(6));
// 	prepend_node(&head, create_node(10));
// 	append_node(&head, create_node(7));

// 	remove_node(&head, nodeToElimin);

// 	print_list(head);
// }
