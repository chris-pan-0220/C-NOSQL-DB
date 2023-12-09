#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* foo(){
    char *ptr;
    return ptr;
}

int main(){
    const char* const org = "helloworld";
    void* val = foo();
    val = (char*)malloc(sizeof(char)*10);
    strcpy((char*)val, org);
    printf("%s", (char*)val);
}