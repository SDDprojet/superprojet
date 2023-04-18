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
#include <stdbool.h>

#define SIZE 20




void initRefs(){ //deja tester
    DIR* dir = opendir(".refs");
    if(dir == NULL){
        int ret = system("mkdir .refs");
        if(ret == -1){
            printf("Erreur lors de la création du répertoire .refs\n");
            return;
        }
        ret = system("touch .refs/master");
        if(ret == -1){
            printf("Erreur lors de la création du fichier .refs/master\n");
            return;
        }
        ret = system("touch .refs/HEAD");
        if(ret == -1){
            printf("Erreur lors de la création du fichier .refs/HEAD\n");
            return;
        }
        return;
    }
    system("touch .refs/master");
    system("touch .refs/HEAD");
    closedir(dir);
}

void createUpdateRef(char* ref_name, char* hash){ //deja testé
    if(ref_name == NULL || hash == NULL){
        return;
    }
    if(file_exists2(ref_name)){
        char command1[256];
        sprintf(command1,"touch .refs/%s",ref_name);
        int ret = system(command1);
        if(ret == -1){
            printf("Erreur lors de la création du fichier .refs/%s\n", ref_name);
            return;
        }
        char command2[256];
        sprintf(command2,"echo %s > .refs/%s",hash,ref_name);
        ret = system(command2);
        if(ret == -1){
            printf("Erreur lors de l'écriture dans le fichier .refs/%s\n", ref_name);
            return;
        }
    }
    char command3[256];
    sprintf(command3,"echo %s > .refs/%s",hash,ref_name);
    int ret = system(command3);
    if(ret == -1){
        printf("Erreur lors de l'écriture dans le fichier .refs/%s\n", ref_name);
    }
}
void deleteRef(char* ref_name){ //déjà testé 
    if(ref_name == NULL){
        return;
    }
    char path[100];
    sprintf(path,".refs/%s",ref_name);
    if(file_exists2(path) == 0){
        printf("Le fichier .refs/%s n'existe pas, il n'y a rien à supprimer!\n", ref_name);
        return;
    }
    char command[256];
    sprintf(command,"rm .refs/%s",ref_name);
    int ret = system(command);
    if(ret == -1){
        printf("Erreur lors de la suppression du fichier .refs/%s\n", ref_name);
        return;
    }
}
char* getRef(char* ref_name){ //déjà testé
    if(ref_name == NULL){
        return NULL;
    }
    char path[100];
    sprintf(path,".refs/%s",ref_name);
    if(file_exists2(path) == 0){
        printf("Le fichier .refs/%s n'existe pas!\n", ref_name);
        return NULL;
    }
    FILE* f = fopen(path,"r");
    if(f == NULL){
        printf("Erreur d'ouverture du fichier .refs/%s\n", ref_name);
        return NULL;
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



void createFile(char* file) { //déjà testé
    if(file == NULL) {
        printf("Le nom de fichier est NULL!\n");
        return;
    }
    char buff[256];
    sprintf(buff,"touch %s",file);
    if(system(buff) == -1) {
        printf("Erreur lors de la création du fichier %s\n", file);
    }
}
 
void myGitAdd(char* file_or_folder) {
    WorkTree* wt = NULL;
    if(file_exists2(".add") == 0) {
        createFile(".add");
        wt = initWorkTree();
    } else {
        wt = ftwt(".add");
    }
    if(file_exists2(file_or_folder) == 1) {
        char* hash = sha256file(file_or_folder);
        appendWorkTree(wt, file_or_folder, hash, 0);
        wttf(wt, ".add");
        free(hash);
    } else {
        printf("Fichier/répertoire %s n'existe pas\n", file_or_folder);

    }
    free(wt);
}

void myGitCommit(char *branch_name, char *message)
{
  if(!file_exists(".refs")){
    printf("Il faut d'abord initialiser les références du projets");
    return;
  }

  char path[256] = ".refs/";
  strcat(path, branch_name);
  if(!file_exists(path)){
    printf("La branche %s n'existe pas", path);
    return;
  }

  char *last_hash = getRef(branch_name);
  char *head_hash = getRef("HEAD");

  bool is_different = strcmp(last_hash, head_hash);

  free(head_hash);

  if(is_different){
    printf("HEAD doit pointer sur le dernier commit de la branche %s", branch_name);
    return;
  }

  WorkTree *wt = ftwt(".add");

  if(wt == NULL){
    printf("Il faut crée le fichier .add");
    return;
  }

  char *hash = saveWorkTree(wt, ".");
  Commit *c = createCommit(hash);
  free(hash);

  if (strlen(last_hash) > 0){
    commitSet(c, "predecessor", last_hash);
  }
  if(message != NULL){
    commitSet(c, "message", message); // commit["message"] = message;
  }
  free(last_hash);

  char *commit_hash = blobCommit(c);
  createUpdateRef(branch_name, commit_hash);
  createUpdateRef("HEAD", commit_hash);

  free(commit_hash);
  freeCommit(c);
  
}

