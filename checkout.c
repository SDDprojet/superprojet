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
#define SIZE 20



void myGitCheckoutBranch(char* branch){
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
void restoreCommit(char* hash_commit){
    restoreWorkTree(hash_commit);


}

void myGitCheckoutBranch(char* branch){

}

List* filterList(List* L, char* pattern){

}

void myGitCheckoutCommit(char* pattern){

}
