#include "checkout.h"
#include "worktf.h"
#include "filehash.h"
#include "liste.h"
#include "commit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <iostream>
#define SIZE 20



void myGitCheckoutBranch(char* branch){ // à refaire
    //on modie le fichier .current_branch 
    char* text = malloc(sizeof(char)*256);
    sprintf(text,"echo %s > .current_branch",branch);
    system(text);
    free(text);
    //modifie la référence HEAD contenant le dernier hash du dernier commit de branch
    char* text2 = malloc(sizeof(char)*256);
    char* hash = hash(branch);
    sprintf(text2,"echo %s > HEAD",hash);
    system(text2);
    free(text2);
    free(hash);

    //restaurer le worktree correspondant au dernier commit de branch 
    char* hash_commit = getRef(branch);
    createUpadateRef("HEAD",hash_commit);
    restoreCommit(hash_commit);
}
void restoreCommit(char* hash_commit){ // à tester
    char* current = getCurrentBranch();

    List* list_branch = branchList(current);

    Cell* c = searchList(list_branch, hash_commit);

    if(c == NULL){
        printf("hash_commit n'existe pas \n");
        return 0;
        free(list_branch);
        free(current);
    }

    char* path = hashToPath(c);
    Worktree* wt = ftwt(hashToPath(commitGet(ftc(path), "tree")));
    restoreWorkTree(wt,".");

}

int commence_meme(char* c1, char* c2){
    int i = 0;
    if(strlen(c1) < strlen(c2)){
        return 0;
    }
    while(c2[i] != '\0' && c1[i] != '\0'){
        if (c2[i] != c1[i]){ // Correction : la condition pour les caractères différents était incorrecte
            return 0; // les chaines ne sont pas identiques
        }
        i++;
    }
    return 1;
}

List* filterList(List* L, char* pattern){ // à faire
    List* l = initList();
    while(L != NULL){
        if(commence_meme(L->data,pattern)== 1){
            insertFirst(l, buildCell(L->data));
        }
        L = L->next 
    }
    return l;
}



void myGitCheckoutCommit(char* pattern){ // à faire
    //recupere tous les commits 
    List* list_commit = getAllCommit();
    //filtre la liste
    List* filter_list = filterList(list_commit,pattern);
    
    if(size_list(filter_list) == 1 ){ // si il y a un seul élément
        createUpdateRef("HEAD",filter_list->data);
    }

    

}
