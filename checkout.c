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
		perror("erreur : hash est null \n");
		return NULL;
	}

	return strcat(hashToPath(hash),".t");;
}

void restoreCommit(char*hash_commit){
	//Si NULL ou si pas de commit
	if((hash_commit == NULL) || strcmp(hash_commit," ") == 0){
		return ;
	}

	char* cur_branch = getCurrentBranch();

	List* list_commit = branchList(cur_branch);

	Cell* commit = searchList(list_commit,hash_commit);

	if(commit == NULL){
		return;
	}
	//si commit n'est pas null
	char* path_commit = hashToPath(commit->data); 
	WorkTree* wt = ftwt(hashToPath(commitGet(ftc(path_commit), "tree")));
	restoreWorkTree(wt,".");
	
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

	createUpdateRef("HEAD",getRef(branch));
	char* ref = getRef("HEAD");
	restoreCommit(ref);


}

List *filterList(List * L,  char *pattern){
    List *new = initList();

    int len = strlen(pattern);
	if(len>0){
		for(Cell *c = *L; c != NULL; c = c->next){
			if(strncmp(c->data, pattern, len)==0){
				Cell *new_cell = buildCell(c->data);
				insertFirst(new, new_cell); //inserer si il commence par pattern (filtrage)
			}
		}
	}
    return new;
}

int sizeList(List *L){
    int len = 0;

    for(Cell *c = *L; c != NULL; c = c->next) 
        len++;

    return len;
}

void myGitCheckoutCommit( char* pattern){
	List *L = getAllCommits();
	
	List *nvlist = filterList(L, pattern); //creer la liste filtrée
	int len = sizeList(nvlist);
	if (len == 1){ //si il ya un seul hash 
		char *commit_hash = (*nvlist)->data;
		printf("off");
		createUpdateRef("HEAD", commit_hash);
		
		restoreCommit(commit_hash);
	}
	else if (len == 0){ // si il y a aucun hash
		printf("Aucun hash ne commence par...\n");
	}
	else { //si il  y en  a plusieurs , on fait affichage
		printf("Plusieurs hash commencent"); 
		for(Cell *c = *nvlist; c; c = c->next)
		printf(" -> %s\n", c->data);
	}

	freeList(L);
	freeList(nvlist);
}
