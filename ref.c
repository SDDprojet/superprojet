#include "ref.h"
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
void myGitCommit(char* branch_name, char* message){
    if(file_exists(".refs") == 0){
        printf("Initialiser d'abord les références du projet \n");
        return;
    }
    //comparer si head et branch name pointent vers la memem chose 
    char[256] ch;
    sprintf(ch,".refs/%s",branch_name);
    if(file_exists(ch) == 0){
        printf("la branch n'existe pas \n");
        return;
    }
    char* bn_ref = getRef(branch_name);
    char* head_ref = getRef("HEAD");
    if(strcmp(bn_ref,head_ref) != 0){
        printf("HEAD doir pointer sur le dernier commit de la branch \n");
        return ;
    }
    //charger worktree puis le supprimer
    WorkTree* wt = ftwt(".add");
    system("rm .add");
    //enregistrer instantané de ce worktree et recuperer son hash
    char* hashwt = saveWorkTree(wt,".");
    //creer commit c
    Commit* c = createCommit(hashwt);
    //lire le fichier branch_name
    char*  predecessor = getRef(branch_name);
    
    if(strlen(predecessor) != 0){
        commitSet(c,"predecessor",bn_ref);
    }
    if(message != NULL){
        commitSet(c,"message",message);
    }
    char *commit_hash = blobCommit(c);
	createUpdateRef(branch_name, commit_hash);
	createUpdateRef("HEAD", commit_hash);


}
    
