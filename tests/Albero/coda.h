#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

typedef struct array {
	size_t count; // currently used slots
	size_t slots; // total slots
	size_t slotSize; // size of a slot
	void *arr; // the actual array
} Array;

Array *create_array(size_t slotDim, size_t dim);
void append(Array *vec, void *elem);
void unshift(Array *vec, void *elem);
void *getElem(Array *vec, size_t pos);