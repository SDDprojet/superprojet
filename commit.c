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


kvp* createKeyVal(char* key,char* val){ //deja testé
    kvp* k = malloc(sizeof(kvp));
    k->key = strdup(key);
    k->value = strdup(val);
    if(k==NULL){
        printf("Erreur d'allocation mémoire");
        return NULL;
    }
    if(k->key==NULL){
        printf("Erreur d'allocation mémoire");
        free(k);
        return NULL;
    }
    if(k->value==NULL){
        printf("Erreur d'allocation mémoire");
        free(k->key);
        free(k);
        return NULL;
    }
    return k ;
}

void freeKeyVal(kvp* kv){
    if(kv != NULL){
        free(kv->key);
        free(kv->value);
        free(kv);
    }
}

char* kvts(kvp* k){ //deja testé
	if(k == NULL){
		return NULL;
	}
	char *chaine = (char *)malloc(sizeof(char)*255);
	snprintf(chaine,255,"%s :%s",k->key,k->value);
	return chaine;
    if(chaine==NULL){
         printf("Erreur d'allocation mémoire : kvts");
         return NULL;
   }
}

kvp* stkv(char* s){ //deja testé
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

Commit* initCommit(){ //deja testé
    Commit* c = malloc(sizeof(Commit));
    if(c==NULL){
         printf("Erreur d'allocation mémoire");
         return NULL;
    }
    c->size = SIZE;
    c->T = malloc(SIZE*sizeof(kvp*));
    if(c->T==NULL){
         printf("Erreur d'allocation mémoire");
         free(c);
         return NULL;
    }
    for(int i = 0; i< SIZE ; i++ ){
        c->T[i] = NULL;
    }
    c->n = 0;
    return c;
}

unsigned long hash(unsigned char *str){ //deja testé


	unsigned long hash = 0;
	int c;

	while (c = *str++)
	    hash += c;

	return hash;
    }



void commitSet(Commit* c,char* key, char* value){     //deja testé
    if(key==NULL || value == NULL){
         printf("Erreur : commitSet");
         return;
    }
    unsigned long p = hash(key)%c->size ;
    while(c->T[p] != NULL ) {
        p =(p +1)%c-> size ; //probing lineaire
    }
    
    c->T[p] =createKeyVal(key,value);
    c->n++;
}
Commit* createCommit(char* hash){ //deja testé
    if(hash==NULL){
         printf("Erreur : createCommit");
         return NULL;
   }
    Commit* c =initCommit();
    commitSet(c,"tree",hash);
    return c;
}

char* commitGet(Commit* c,char* key){ //deja testé
    if(c==NULL || key==NULL){
            printf("Erreur : commitGet");
            return NULL;
    }
    int i = 0;
    int p = hash(key)%c->size;
    while(c->T[p] != NULL && i < c->size){
        if(strcmp(c->T[p]->key,key)==0){
            return c->T[p]->value;
        }
        p = (p+1)%c->size;
        i++;
    }
    return NULL;
}

char *cts(Commit *c) { //déjà tester
    int i = 0;
    int chaine_len = 0;
    int max_len = sizeof(char) * 100 * c->n;
    char *chaine = malloc(max_len*sizeof(char));
    if(chaine != NULL){
        memset(chaine,0,max_len*sizeof(char));

    }
    while (i < c->size) {
        if (c->T[i] != NULL) {
            char *temp = kvts(c->T[i]);
            if (chaine_len + strlen(temp) + 1 < max_len) {
                strcat(chaine, temp);
                strcat(chaine, "\n");
                chaine_len += strlen(temp) + 1;
            } else {
                printf("Erreur: dépassement de tampon\n");
                break;
            }
            free(temp);
        }
        i++;
    }
    return chaine;
}


Commit *stc(char *s){ //déjà tester 
    Commit *c = initCommit();   
    char *token = strtok(s, "\n");

    while(token){
        kvp* pair = stkv(token);

    commitSet(c, pair->key, pair->value);
    freeKeyVal(pair);
    token = strtok(NULL, "\n");
    }
    free(token);
    return c;  
}


void ctf(Commit *c, char *file)
{
  FILE *f = fopen(file, "w");
  if(f == NULL){
    printf("probleme d'ouverture de fichier \n");
    return;
  }

  char *s = cts(c);
  fputs(s, f);
  free(s);
  fclose(f);
}

Commit *ftc(const char *file){ //déjà tester
  FILE *f = fopen(file, "r");

  if(f == NULL){
    printf("Problème d'ouverture fichier \n");
    return NULL;
  }

  size_t size = sizeof(char) * 256;
  char *s = malloc(size);
  if(s==NULL){
        printf("Erreur d'allocation de mémoire\n");
        fclose(f);
        return NULL;
  }
  memset(s, 0, size);

  char buf[256];
  while(fgets(buf, 256, f)){
    if(strlen(buf) + strlen(s) >= size){
      size *= 2; s = realloc(s, size);
    }
    strcat(s, buf);
  }

  Commit *c = stc(s);
  fclose(f);
  free(s);
  return c;
}

char *blobCommit(Commit *c)
{
  static char template[] = "/tmp/myfileXXXXXX";
  char fname[100];
  strcpy(fname,template);
  mkstemp(fname);
  
  ctf(c, fname);

  char *hash = sha256file(fname);
  char *chemin = hashToPath(hash);

  strcat(chemin,".c") ;
  cp(chemin, fname);

  remove(fname);
  return hash;
}
