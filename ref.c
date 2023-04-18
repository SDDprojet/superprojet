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

void createUpdateRef( char *ref_name, char *hash)
{
  char path[256] = ".refs/";

  if(ref_name == NULL){
    printf( "Chaine de charactère null...");
    return;
  }

  strcat(path, ref_name);
  FILE *f = fopen(path, "w");

  if(f == NULL){
    printf("Problème lors de l'ouverture du fichier %s", path);
  }

  fputs(hash, f);

  fclose(f);
}
void deleteRef(char* ref_name){ //déjà testé 
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
	//printf("ENTER getRef\n");

	// Permet de rentrer dans le repertoire .refs
	char buff_path_to_ref[255];
	sprintf(buff_path_to_ref,".refs/%s",ref_name);
	FILE* f = fopen(buff_path_to_ref,"r");
	//printf("PATH =%s\n",buff_path_to_ref);
	if(f == NULL){
		//printf("PAS DE BOL\n");
		return NULL;
	}
	char *buff_hash = (char*)malloc(sizeof(char)*255);
	if(fgets(buff_hash,200,f) == NULL){
		char* buff_vide = malloc(sizeof(char)*2);
		sprintf(buff_vide," ");
		fclose(f);
		//printf("àà:%s:\n",buff_vide);
		return buff_vide;
	}
	//printf("%s",buff_hash);
	fclose(f);
	//printf("àà:%s\n",buff_hash);
	return buff_hash;
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

