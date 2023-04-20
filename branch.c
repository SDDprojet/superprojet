#include "worktf.h"
#include "commit.h"
#include "filehash.h"
#include "branch.h"
#include "liste.h"
#include "ref.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#define SIZE 20



void initBranch(){
    //initialise une branch
    FILE* f = fopen(".current_branch", "w");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fputs("master", f);
    fclose(f);
}

int branchExists(char* branch){
    //verifie si la branch existe
    List* chemin=listdir(".refs");
    return searchList(chemin,branch) !=NULL;
}

void createBranch(char* branch){
    //cree une branch
    char* hash=getRef("HEAD");
    createUpdateRef(branch,hash);
}

char* getCurrentBranch(){
    //renvoi la branch courante
    FILE* f = fopen(".current_branch","r");
    if (f == NULL){
        printf("Erreur d'ouverture du fichier .current_branch.\n");
        return NULL;
    }
    char* buff = malloc(sizeof(char) * 100);
    if (buff == NULL){
        printf("Erreur d'allocation de mémoire.\n");
        fclose(f);
        return NULL;
    }
    fscanf(f, "%99s", buff); //%99s
    fclose(f);
    return buff;
}

char* hashTopathCommit(char* hash){
    //transforme un hash en path 
    if (hash == NULL){
        printf("Erreur : pointeur NULL passé en argument.\n");
        return NULL;
    }

    char* buff = malloc(sizeof(char) * 100);
    if (buff == NULL){
        printf("Erreur d'allocation de mémoire.\n");
        return NULL;
    }

    char* hashPath = hashToPath(hash);
    if (hashPath == NULL){
        printf("Erreur lors de la conversion du hash en chemin.\n");
        free(buff); 
        return NULL;
    }

    sprintf(buff, "%s.c", hashPath);
    free(hashPath); 
    return buff;
}

void printBranch(char *branch){
    //affiche les messages des commits effectuer sur la branch et la ref de la branch
  char *commit_hash = getRef(branch);
  char *path = commitPath(commit_hash);

  Commit *c = ftc(path);

  if(path) free(path);

  while(c != NULL){
    char *msg = commitGet(c, "message");
    char *prev = commitGet(c, "predecessor");
    if(msg) {
      printf("%s => \"%s\"\n", commit_hash, msg);
      free(msg);
    }

    else printf("%s\n", commit_hash);

    if(prev){
      commit_hash = strdup(prev);
      free(prev);
      path = commitPath(commit_hash);

      if(commit_hash) free(commit_hash);
      freeCommit(c);
      c = ftc(path);
      if (c == NULL){
        printf("ftc a renvoyé null..");
        if(path) free(path);
        break;
      }

      if(path) free(path);
    }
    else {
      freeCommit(c);
      c = NULL;
    }
  }
}

List* branchList(char* branch){
    List* L=initList();
    char* commit_hash=getRef(branch);
    Commit* c=ftc(hashTopathCommit(commit_hash));
    while(c!=NULL){
        insertFirst(L,buildCell(commit_hash));
        if(commitGet(c,"predecessor")!=NULL){
            commit_hash=commitGet(c,"predecessor");
            c=ftc(hashTopathCommit(commit_hash));
        }else{
            c=NULL;
        }
    }
    return L;
}



List* getAllCommits(){
    List* L = initList();
    List* content = listdir(".refs");
    if (content == NULL){
        printf("Erreur lors de la récupération du contenu du répertoire : gaC'.refs'.\n");
        return NULL;
    }

    for(Cell* ptr = *content; ptr != NULL; ptr = ptr->next) {
        if (ptr->data[0] == '.')
            continue;
        List* list = branchList(ptr->data);
        if (list == NULL){
            printf("Erreur lors de la récupération de la liste des commits pour la branche gaC'%s'.\n", ptr->data);
            freeList(content);
            freeList(L);
            return NULL;
        }

        Cell* cell = *list;
        while(cell != NULL ) {
            if (searchList(L, cell->data) == NULL) {
                insertFirst(L, buildCell(cell->data));
            }  
            cell = cell->next;
        }

        freeList(list); 
    }

    freeList(content); 
    return L;
}
