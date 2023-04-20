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
//initialise une ref
    DIR* dir = opendir(".refs");
    if(dir == NULL){
        int ret = system("mkdir .refs");
        if(ret == -1){
            printf("Erreur lors de la création du répertoire .refs : initref\n");
            return;
        }
        ret = system("touch .refs/master");
        if(ret == -1){
            printf("Erreur lors de la création du fichier .refs/master : initref\n");
            return;
        }
        ret = system("touch .refs/HEAD");
        if(ret == -1){
            printf("Erreur lors de la création du fichier .refs/HEAD:initref\n");
            return;
        }
        return;
    }
    system("touch .refs/master");
    system("touch .refs/HEAD");
    closedir(dir);
}

void createUpdateRef( char *ref_name, const char *hash){//deja testé
//cree ou met a jour une ref
  
  char path[256] = ".refs/";

  if(ref_name == NULL){
    printf( "Chaine de charactère null...: createUpdateRef");
    return;
  }

  strcat(path, ref_name);
  FILE *f = fopen(path, "w");

  if(f == NULL){
    printf("Problème lors de l'ouverture du fichier : createUpdateref%s", path);
  }
  
  fputs(hash, f);

  fclose(f);
}
void deleteRef(char* ref_name){ //déjà testé 
//suprime une ref
    if(ref_name == NULL){
        return;
    }
    char path[100];
    sprintf(path,".refs/%s",ref_name);
    if(file_exists(path) == 0){
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
char* getRef(char* ref_name){
//retourne la ref 

	// Permet de rentrer dans le repertoire .refs
  char buff[256] = ".refs/";
  strcat(buff, ref_name);

  FILE *f = fopen(buff, "r");

  if(f == NULL){
    printf( "Problème lors de la lecture du fichier %s", buff);
    return NULL;
  }

  char *s = malloc(sizeof(char) * 256);
  memset(s, 0, 256);

  fgets(s, 256, f);
  fclose(f);
  return s;
}

void createFile(char* file) { //déjà testé
//cree un fichier 
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
//ajoute un fichier ou un repertoire dans une zone de preparation
    WorkTree* wt = NULL;
    if(file_exists(".add") == 0) {
        createFile(".add");
        wt = initWorkTree();
    } else {
        wt = ftwt(".add");
    }
    if(file_exists(file_or_folder) == 1) {
        char* hash = sha256file(file_or_folder);
        appendWorkTree(wt, file_or_folder, hash, 0);
        wttf(wt, ".add");
        free(hash);
    } else {
        printf("Fichier/répertoire %s n'existe pas\n", file_or_folder);

    }
    free(wt);
}

void myGitCommit(char *branch_name, char *message){
  //cree un commit dans branch_name avec le message
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

  char *predecessor = getRef(branch_name);
  char *head_hash = getRef("HEAD");

  bool est_different = strcmp(predecessor, head_hash);

  free(head_hash);

  if(est_different){
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

  if (strlen(predecessor) > 0){
    commitSet(c, "predecessor", predecessor);
  }
  if(message != NULL){
    commitSet(c, "message", message); 
  }
  free(predecessor);

  char *commit_hash = blobCommit(c);
  createUpdateRef(branch_name, commit_hash);
  createUpdateRef("HEAD", commit_hash);

  free(commit_hash);
  freeCommit(c);
  
}

