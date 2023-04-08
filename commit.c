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


kvp* createKeyVal(char* key,char* val){
    kvp* k = malloc(sizeof(kvp));
    k->key = strdup(key);
    k->value = strdup(val);
    return k ;
}

void freeKeyVal(kvp* kv){
    free(kv->key);
    free(kv->value);
    free(kv);
}

char* kvts(kvp* k){
	if(k == NULL){
		return NULL;
	}
	char *chaine = (char *)malloc(sizeof(char)*255);
	sprintf(chaine,"%s :%s",k->key,k->value);
	return chaine;
}

kvp* stkv(char* s){
    int r=0;
    char mot1[100];
    char mot2[100];
    int i=0;
    while (s[r]!=':'){
           mot1[r] = s[r];
           r++ ;
    }

    mot1[r]='\0';
        r++;
    while (s[r]!='\0'){
        mot2[i]=s[r];
        i++;
        r++;
    }
    
    mot2[i]='\0';
    kvp* k=createKeyVal(mot1,mot2);
    return k;
}

Commit* initCommit(){
    Commit* c = malloc(sizeof(Commit));
    c-> n = 0;
    c->size = SIZE;
    c->T = malloc(sizeof(kvp));
    for(int i = c->n; i< c->size ; i++ ){
        c->T[i]->key = NULL;
        c->T[i]->value = NULL;
    }
    return c;
}

unsigned long hash(unsigned char *str){


	unsigned int hash = 0;
	int c;

	while (c = *str++)
	    hash += c;

	return hash;
    }



void commitSet(Commit* c,char* key, char* value){    
    int p = hash(key)%c->size ;
    while(c->T[p] != NULL ) {
        p =(p +1)%c-> size ; //probing lineaire
    }
    c->T[p] =createKeyVal(key,value);
    c->n++;
}
Commit* createCommit(char* hash){
    Commit* c =initCommit();
    commitSet(c,"tree",hash);
    return c;
}

char* commitGet(Commit* c,char* key){
    int i = 0;
    while(i != c->n){
        if(strcmp(c->T[i]->key,key)==0){
            return c->T[i]->value;
        }
        i++;
    }
    return NULL;
}

char* cts(Commit* c){
    int i =0;
    char* chaine;
    while(i != c->n){
        sprintf(chaine,"%s \n",kvts(c->T[i]));
    }
    return chaine ;
}

void ctf(Commit* c, char* file){
    FILE* dest = fopen(file,'w');
    if(dest == NULL){
        printf("Problème d'ouverture fichier \n");
        return ;
    }
    int i = 0;
    while(i<c->n){
        kvp* chaine = c->T[i];
        fprintf(dest,"%s\n", kvts(chaine));
        free(chaine);
        i++;
    }
    fclose(dest);
}

    


//Commit* ftc(char* file){
//    Commit c = initCommit();
//    return c;
//} 