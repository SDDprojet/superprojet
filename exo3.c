#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

List* stol(char* str){
    List* l = initList();
    char* token = strtok(str,"|");
    while(token != NULL){
        Cell* c = buildCell(token);
        insertFirst(l,c);
        token = strtok(NULL,"|");
    }
    return l;
}