#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>


struct node {
	void *value;
	struct node *next;
};


struct node *create_node(void *v){
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



void print_int_list(struct node *head){
	while(head){
		printf("%d \n", *((int*)head->value));
		head = head->next;
	}
}


// int main(){
// 	struct node *head = NULL;
// 	int values[] = {1, 2, 3, 4, 5, 6, 7};
	
// 	for(int i = 0; i < 7; i++){
// 		struct node *new_node = create_node(&values[i]);
// 		append_node(&head, new_node);
// 	}

// 	print_list(head);
// }
