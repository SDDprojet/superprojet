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

void restoreCommit(char*hash_commit){
	//2eme partie du ou debug de 1h
	if((hash_commit == NULL) || strcmp(hash_commit," ") == 0){//Si NULL ou si pas de commit
		return ;
	}

	char* cur_branch = getCurrentBranch();

	List* list_commit = branchList(cur_branch);

	Cell* commit = searchList(list_commit,hash_commit);

	if(commit == NULL){
		return;
	}

	char* path_commit = hashToPath(commit->data); //commit->data  avec le point c (a creer commit_to_path)
	WorkTree* wt_commit = ftwt(hashToPath(commitGet(ftc(path_commit), "tree")));
	restoreWorkTree(wt_commit,".");
	
}
void myGitCheckoutBranch(char* branch){
	if(branch == NULL){
		return;
	}

	FILE* f = fopen(".current_branch","w");
	if(f == NULL){
		return;
	}

	fprintf(f,"%s",branch);
	fclose(f);
	printf("Déplacement dans %s\n",branch);

	//printf("Cas -6");
	createUpdateRef("HEAD",getRef(branch));
	char* ref = getRef("HEAD");
	restoreCommit(ref);
	//free(ref);

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
