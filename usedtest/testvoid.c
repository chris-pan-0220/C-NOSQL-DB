#include <stdio.h>
#include <stdlib.h>

typedef struct Node{
    int data;
}Node;


void handle(Node *node){
    return;
}

void foo(void *ptr){
    return handle(ptr);
}


int main(){
    Node *node = (Node*)malloc(sizeof(Node));
    foo(node);
}