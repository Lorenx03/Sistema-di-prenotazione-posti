#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "coda.h"

// typedef struct array {
// 	size_t count; // currently used slots
// 	size_t slots; // total slots
// 	size_t slotSize; // size of a slot
// 	void *arr; // the actual array
// } Array;


Array *create_array(size_t slotDim, size_t dim){
    Array *vec = malloc(sizeof(Array));
    if (vec == NULL) return NULL;
    
    vec->count = 0;
    vec->slots = dim;
    vec->slotSize = slotDim;
    vec->arr = calloc(dim, slotDim);
    if (vec->arr == NULL) return NULL;

    return(vec);
}


void append(Array *vec, void *elem){
    if (vec == NULL || elem == NULL) return;

	if (vec->count + 1 > vec->slots) {
		if(realloc(vec->arr, vec->count * vec->slotSize + 50) == NULL) return;
	}
	
	memcpy(((char*)vec->arr) + (vec->count) * vec->slotSize, elem, vec->slotSize);
	
	vec->count++;
}


void unshift(Array *vec, void *elem){
	if (vec == NULL || vec->count == 0) {
		elem = NULL;
		return;
	}

	memcpy(elem, ((char*)vec->arr), vec->slotSize);
	memmove(vec->arr, ((char*)vec->arr) + vec->slotSize, vec->slotSize * (vec->count - 1));
	vec->count--;
}


void *getElem(Array *vec, size_t pos){
	if (vec == NULL || pos > vec->count) return NULL;
	return (((char*)vec->arr) + vec->slotSize * pos);
}



// int main() {
//     Array *vec = create_array(sizeof(int), 2);
	
// 	int arr[5];

// 	for (size_t i = 0; i < 5; i++) {
// 		arr[i] = i;
// 		append(vec, (arr + i));
// 	}

// 	for (size_t i = 0; i < vec->count; i++) {
// 		printf("%d ", *((int*)getElem(vec, i)));
// 	}
	
// 	printf("\n");

// 	int val;
// 	unshift(vec, &val);
// 	unshift(vec, &val);

// 	for (size_t i = 0; i < vec->count; i++) {
// 		printf("%d ", *((int*)getElem(vec, i)));
// 	}
// }
