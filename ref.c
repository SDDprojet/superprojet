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

void initRefs(){ // déjà tester 
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

void createUpdateRef(char* ref_name, char* hash){ //déjà tester
    if(ref_name == NULL || hash == NULL){
        return;
    }
    if(file_exists(ref_name) == 0){
        char command1[256];
        sprintf(command1,"touch .refs/%s",ref_name);
        system(command1);
        char command2[256];
        sprintf(command2," echo %s > .refs/%s",hash,ref_name);
        system(command2);
    }
    char command3[256];
    sprintf(command3,"echo %s > .refs/%s",hash,ref_name);
    system(command3);
}

void deleteRef(char* ref_name){ //déjà tester
    if(ref_name == NULL){
        return ;
    }
    char path[100];
    sprintf(path,".refs/%s",ref_name);
    if(file_exists2(path) == 0){
        printf("fichier n'existe pas, il n'y a pas à supprimer! \n");
        return ;
    }
    char command[256];
    sprintf(command,"rm .refs/%s",ref_name);
    system(command);
    return;

}
char* getRef(char* ref_name){ //marhce
    if(ref_name == NULL){
        return ;
    }
    char path[100];
    sprintf(path,".refs/%s",ref_name);
    if(file_exists2(path) == 0){
        printf("fichier n'existe pas \n");
        return ;
    }
    FILE* f = fopen(path,"r");
    if(f == NULL){
        printf("erreur d'ouverture de ficher \n");
        return;
    }
    char ligne[256];
    fgets(ligne,256,f);
    fclose(f);
    int len = strlen(ligne);
    if (len > 0 && ligne[len-1] == '\n') {
        ligne[len-1] = '\0';
    }
    char* ref = malloc(strlen(ligne)+1);
    strcpy(ref, ligne);
    return ref;

}

void createFile(char* file) {
    char buff[256];
    sprintf(buff,"touch %s",file);
    system(buff);
}
 
void myGitAdd ( char* file_or_folder ){
    WorkTree * wt ;
    if (file_exists2(".add")== 0 ){
        createFile (".add") ;
        wt = initWorkTree () ;
    }else{
        wt = ftwt (".add") ;
    }
    if(file_exists2(file_or_folder) == 1){
        appendWorkTree(wt,file_or_folder,0,NULL) ;
        wttf(wt,". add") ;
    }else{
        printf("fichier/repertpoire %s n'existe pas \n",file_or_folder);
    }
}

void myGitCommit(char* branch_name, char* message){
    if(file_exists(".refs") == 0){
        printf("Initialiser d'abord les références du projet \n");
        return;
    }
    //comparer si head et branch name pointent vers la memem chose 
    char ch[256];
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
    
