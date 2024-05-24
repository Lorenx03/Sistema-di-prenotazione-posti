#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/linkedList.h"


void print_list(struct node *head){
	while(head){
		printf("%d \n", head->value);
		head = head->next;
	}
}

int main(){
	struct node *head = NULL;
	struct node *nodeToElimin = create_node(2);
	append_node(&head, nodeToElimin);
	append_node(&head, create_node(3));
	append_node(&head, create_node(4));
	append_node(&head, create_node(5));
	append_node(&head, create_node(6));
	prepend_node(&head, create_node(10));
	append_node(&head, create_node(7));
	printf("Hello");

	remove_node(&head, nodeToElimin);

	print_list(head);
}