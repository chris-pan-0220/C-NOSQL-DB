#include <string.h>
#include <stdio.h>

int main(){
    const char * const ptr = "";
    printf("string len: %ld", strlen(ptr));
}