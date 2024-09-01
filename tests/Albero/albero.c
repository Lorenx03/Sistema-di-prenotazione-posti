#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "coda.h"

typedef struct item {
    int value;
} Item;


typedef struct node {
    Item *item;
    struct node *parent;
    struct node *sibling;
    struct node *child;
} Node;


typedef struct tree {
    Node *root;
} Tree;


//=============================> Costruttori <==============================

Item *create_item(int val) {
    Item *item;
    item = malloc(sizeof(Item));
    item->value = val;

    return (item);
}

Node *create_node() {
    Node *node;
    node = malloc(sizeof(Node));

    node->item = NULL;
    node->parent = NULL;
    node->sibling = NULL;
    node->child = NULL;

    return (node);
}

Tree *create_tree() {
    Tree *albe;
    albe = malloc(sizeof(Tree));
    albe->root = NULL;

    return albe;
}



//=============================> Riempimento <==============================
void appendChild(Tree *tree, Node *subTreeRoot, Node *newChild) {
    if(tree->root == NULL){
        tree->root = newChild;
        return;
    }

    newChild->parent = subTreeRoot;

    if (subTreeRoot->child == NULL){
        subTreeRoot->child = newChild;
    }else{
        Node *currentNode = subTreeRoot->child;
        while (currentNode->sibling != NULL) {
            currentNode = currentNode->sibling;
        }
        currentNode->sibling = newChild;
    }
}




void printChilds(Node *node){
    if(node == NULL) return;
    Node *currentChild = node->child;

    while(currentChild != NULL) {
        printf("%d ", currentChild->item->value);
        currentChild = currentChild->sibling;
    }
    printf("\t");
}

bool hasChilds(Node *node){
    return node->child != NULL;
}

void printTree(Tree *tree){
    if(tree == NULL) return;
    if(tree->root == NULL) return;

    int level = 0;
    printf("[%d] \t %d \n", level, tree->root->item->value);
    if(tree->root->child == NULL) return;

    Node *currentNode = tree->root;
    Array *queue = create_array(sizeof(Node), 5);
    append(queue, currentNode);
    level++;
    int iter = 0;

    while (queue->count > 0){
        iter = queue->count;
        //printf("[%d]", iter);
        printf("[%d] \t", level);
        for (int i = 0; i < iter; i++) {
            Node nodeToCheck;
            unshift(queue, &nodeToCheck);

            currentNode = nodeToCheck.child;
            while (currentNode != NULL) {
                if (hasChilds(currentNode)) {
                    append(queue, currentNode);
                }
                currentNode = currentNode->sibling;
            }
            printChilds(&nodeToCheck);
        }
        
        level++;
        printf("\n");
    }
}


int main() {
    Tree *albero;
    albero = create_tree();

    Node *testNode = create_node();
    testNode->item = create_item(10);

    Node *testNode2 = create_node();
    testNode2->item = create_item(20);

    Node *testNode3 = create_node();
    testNode3->item = create_item(30);

    Node *testNode4 = create_node();
    testNode4->item = create_item(40);

    Node *testNode5 = create_node();
    testNode5->item = create_item(50);

    Node *testNode6 = create_node();
    testNode6->item = create_item(60);

    Node *testNode7 = create_node();
    testNode7->item = create_item(70);

    Node *testNode8 = create_node();
    testNode8->item = create_item(80);

    //printf("%d \n", testNode->item->value);

    appendChild(albero, albero->root, testNode);
    appendChild(albero, albero->root, testNode2);
    appendChild(albero, albero->root, testNode3);
    appendChild(albero, albero->root->child, testNode4);
    appendChild(albero, albero->root->child, testNode5);
    appendChild(albero, albero->root->child->sibling, testNode6);
    appendChild(albero, albero->root->child->sibling, testNode7);

    //albero->root = testNode;
    // printf("%d ", albero->root->item->value);
    // printf("%d ", albero->root->child->item->value);
    // printf("%d \n", albero->root->child->sibling->item->value);

    //printChilds(albero->root->child->sibling);
    printTree(albero);

    return 0;
} 
