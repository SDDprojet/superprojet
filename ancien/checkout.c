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


char* workTreePath( char* hash){
	if(hash == NULL){
		perror("Tentative de conversion avec un hash null");
		return NULL;
	}

	return strcat(hashToPath(hash),".t");;
}

void restoreCommit(char *hash){
	char *path = commitPath(hash);
	Commit *c = ftc(path);

	if(c == NULL){
		perror("La fonction ftc a renvoyé NULL");
		return;
	}

	char *wt_hash = commitGet(c, "tree");
	if(wt_hash == NULL){
		perror("Le commit de contien pas tree ?");
		return;
	}

	char *tree_hash = workTreePath(wt_hash);
	if(tree_hash == NULL){
		perror("Impossible de convertir en chemin d'accès...");
		return;
	}

	WorkTree *wt = ftwt(tree_hash);
	if(wt == NULL){
		perror("Problème de conversion en WorkTree du fichier ");
		return;
	}
	
	restoreWorkTree(wt, ".");

	free(path);
	free(tree_hash);
	freeCommit(c);
	freeWorkTree(wt);
}

void myGitCheckoutBranch(char *branch){
	FILE *f = fopen(".current_branch", "w");

	if(f == NULL){
		printf("Le fichier a retourné null...");
		return;
	}
	fprintf(f, "%s", branch);
	fclose(f);

	char* hash_commit = getRef(branch);

	if(hash_commit == NULL){
		printf("Le hash a retourné null...");
		return;
	}

	createUpdateRef("HEAD", hash_commit);
	restoreCommit(hash_commit);

	free(hash_commit);
}

List *filterList(List * L,  char *pattern){
    List *filtered = initList();

    int len = strlen(pattern);
	if(len>0){
		for(Cell *cursor = *L; cursor != NULL; cursor = cursor->next){
			if(strncmp(cursor->data, pattern, len)==0){
				Cell *new_cell = buildCell(cursor->data);
				insertFirst(filtered, new_cell);
			}
		}
	}
    return filtered;
}

int sizeList(List *L){
    int len = 0;

    for(Cell *cursor = *L; cursor != NULL; cursor = cursor->next) 
        len++;

    return len;
}

void myGitCheckoutCommit( char* pattern){
	printf("ggggggggggg\n");
	List *L = getAllCommits();
	
	List *filtered_list = filterList(L, pattern);
	printf("laaaaaa\n");
	int len = sizeList(filtered_list);
	if (len == 1){
		char *commit_hash = (*filtered_list)->data;
		printf("off");
		createUpdateRef("HEAD", commit_hash);
		
		restoreCommit(commit_hash);
	}
	else if (len == 0){
		printf("Aucun hash ne commence par...\n");
	}
	else {
		printf("Plusieurs hash commencent");
		for(Cell *c = *filtered_list; c; c = c->next)
		printf(" -> %s\n", c->data);
	}

	freeList(L);
	freeList(filtered_list);
}
