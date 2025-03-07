#include "linkedList.h"

// typedef struct node {
// 	void *value;
// 	struct node *next;
// } LinkedListNode;


LinkedListNode *create_node(void *v){
	LinkedListNode *new_node = malloc(sizeof(LinkedListNode));

	if(!new_node){
		fprintf(stderr, "Error in memory allocation");
		exit(EXIT_FAILURE);
	}

	new_node->value = v;
	new_node->next = NULL;

	return new_node;
}



void prepend_node(LinkedListNode **head, LinkedListNode *new_node){
	new_node->next = *head;
	*head = new_node;
}



void append_node(LinkedListNode **head, LinkedListNode *new_node){
	if(*head == NULL){
		*head = new_node;
	}else{
		LinkedListNode *currentNode = *head;
		while(currentNode->next){
			currentNode = currentNode->next;
		}

		currentNode->next = new_node;
	}
}



void remove_node(LinkedListNode **head, LinkedListNode *entry){
	while((*head) != entry){
		head = &(*head)->next;
	}
	*head = entry->next;

	free(entry);
}



void print_int_list(LinkedListNode *head){
	while(head){
		printf("%d \n", *((int*)head->value));
		head = head->next;
	}
}


// int main(){
// 	LinkedListNode *head = NULL;
// 	int values[] = {1, 2, 3, 4, 5, 6, 7};
	
// 	for(int i = 0; i < 7; i++){
// 		LinkedListNode *new_node = create_node(&values[i]);
// 		append_node(&head, new_node);
// 	}

// 	print_int_list(head);
// }
