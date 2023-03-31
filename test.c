#include "lexer.c"
#include <stdio.h>
int main(){
    FILE * fp = fopen("input.txt","r");
    getStream(fp);
    buffer b = getBuffer();
    printf(b.one);
}