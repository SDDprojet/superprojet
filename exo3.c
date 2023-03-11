#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

List* stol(char* s){
    List * L = initList();
    int debut = 0, i=0, cpt = 0;
    char *tmp = malloc(256);
    while(s[i]!= '\0'){
        if(s[i]!='|'){
            cpt++;
        }else if (i<strlen(s)){
            tmp = strncpy(tmp, s+debut, cpt);
            Cell * C = buildCell(tmp);
            insertFirst(L,C);

            debut = i+1;
            cpt = 0;
        }
        i++;
    }
    return L;
}