#include "worktf.h"
#include "ref.h"
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

void initRefs(){ // à tester 
    DIR* dir =opendir(".refs");
    if(dir == NULL){
        system("mkdir .refs");
        system("touch .refs/master");
        system("touch .refs/HEAD");
        return;
    }
    system("touch .refs/master");
    system("touch .refs/HEAD");
    closedir(dir);
}

void createUpdateRef(char* ref_name, char* hash): //à tester
    if(ref_name == NULL || hash == NULL){
        return;
    }
    if(file_exists(ref_name) == 0){
        system("mkdir %s",ref_name);
        system(" echo %s > %s",ref_name,hash);
    }
    system(" echo %s > %s",ref_name,hash);

void deleteRef(char* ref_name){ //à tester
    if(ref_name == NULL){
        return ;
    }
    if(file_exists(ref_name) == 0){
        printf("fichier n'existe pas, il n'y a pas à supprimer! \n");
        return ;
    }
    system("rm .ref/%s",ref_name);
    return;

}
char* getRef(char* ref_name){
    if(ref_name == NULL){
        return ;
    }
    if(file_exists(ref_name) == 0){
        printf("fichier n'existe pas \n");
        return ;
    }
    FILE* f = fopen(ref_name,"r");
    if(f == NULL){
        printf("erreur d'ouverture de ficher \n");
        return;
    }
    char[100] ligne;
    fgets(ligne,100,f) ;
    fclose (f) ;

    return ligne;
}

    
