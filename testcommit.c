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
    if(k==NULL){
        printf("Erreur d'allocation mémoire : ckv");
        return NULL;
    }
    k->key = strdup(key);
    if(k->key==NULL){
        printf("Erreur d'allocation mémoire : ckv");
        free(k);
        return NULL;
    }
    k->value = strdup(val);
    if(k->value==NULL){
        printf("Erreur d'allocation mémoire : ckv");
        free(k->key);
        free(k);
        return NULL;
    }
    return k ;
}

void freeKeyVal(kvp* kv){
    if(kv!=NULL){
        free(kv->key);
        free(kv->value);
        free(kv);
    }
}

char* kvts(kvp* k){ //deja testé
	if(k == NULL|| k->key == NULL || k->value == NULL){
		return NULL;
	}
	char *chaine = (char *)malloc(sizeof(char)*255);
   if(chaine==NULL){
         printf("Erreur d'allocation mémoire : kvts");
         return NULL;
   }
	snprintf(chaine,255,"%s :%s",k->key,k->value);
	return chaine;
}

kvp* stkv(char* s){ //deja testé
    int r=0;
    char mot1[100];
    char mot2[100];
    int i=0;
    if (strchr(s, ':') == NULL) {
        return NULL;
    }
    while (s[r]!=':' && r<99){
           mot1[r] = s[r];
           r++ ;
    }

    mot1[r]='\0';
    r++;
    while (s[r]!='\0' && i<99){
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
         printf("Erreur d'allocation mémoire : ic");
         return NULL;
    }
    c->size = SIZE;
    c->T = calloc(SIZE,sizeof(kvp*));
    if(c->T==NULL){
         printf("Erreur d'allocation mémoire : ic");
         free(c);
         return NULL;
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
    if(c==NULL){
         printf("Erreur : commitSet");
         return;
    }
    unsigned long p = hash(key)%c->size ;
    while(c->T[p] != NULL ) {
        p =(p +1)%c-> size ; //probing lineaire
    }
    kvp* kv=createKeyVal(key,value);
    if(kv==NULL){
         printf("Erreur : commitSet");
         return;
    }
    c->T[p] =kv;
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

char* cts2(Commit* c){ //deja testé
    int i =0;
    char* chaine = malloc(sizeof(char)*100*c->n ) ;
    while(i < c->size){
        if(c->T[i] != NULL){
            printf("i = %d et c->n = %d \n",i,c->n);
            strcat(chaine,kvts(c->T[i]));
            strcat(chaine,"\n");
        }
        i++;
    }
    return chaine ;
}
char *cts(Commit *c) {
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
        }
        i++;
    }
    return chaine;
}


Commit *stc(char *ch) {
    int pos = 0;
    int n_pos = 0;
    int sep = '\n';
    char *ptr;
    char *result = (char*)malloc(sizeof(char) * 10000);
    Commit *c = initCommit(10);
    while (pos < strlen(ch)) {
        ptr = strchr(ch + pos, sep);
        if (ptr == NULL)
            n_pos = strlen(ch) + 1;
        else
            n_pos = ptr - ch + 1;
        memcpy(result, ch + pos, n_pos - pos - 1);
        result[n_pos - pos - 1] = '\0';
        pos = n_pos;
        kvp *elem = stkv(result);
        commitSet(c, elem->key, elem->value);
    }
    free(result)
    return c;
}
void ctf(Commit* c, char* file){ //dejà testé 
    FILE* dest = fopen(file,"w");
    if(dest == NULL){
        printf("Problème d'ouverture fichier \n");
        return ;
    }
    int i = 0;
    while(i<c->size){
        if(c->T[i] != NULL){
            kvp* chaine = c->T[i];
            fprintf(dest,"%s\n", kvts(chaine));
            free(chaine->key);
            free(chaine->value);
            free(chaine);
            
        }
        i++;
    }
    fclose(dest);
}

Commit* ftc(char* file) {
    FILE* f = fopen(file, "r"); // Correction du mode d'ouverture du fichier

    if (f == NULL) {
        printf("Problème d'ouverture fichier \n");
        return NULL; // Correction de la gestion d'erreur pour le cas d'échec d'ouverture de fichier
    }

    char buffer[256];
    char* chaine = malloc(sizeof(char) * 256); 
    if(chaine==NULL){
         printf("Erreur d'allocation de mémoire\n");
         fclose(f)
         return NULL;
    }
    chaine[0]='\0';
    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        strcat(chaine, buffer);
    }
    Commit* c = stc(chaine);
    fclose(f);
    free(chaine); 
    return c; 
}
char * blobCommit ( Commit * c ) {
    char fname[100] = "/tmp/myfileXXXXXX" ;
    int fd =mkstemp(fname);
      if(fd == -1){
         printf("Erreur de création de fichier temporaire \n");
         return NULL;
      }
    ctf(c,fname);
    char *hash = sha256file(fname) ;
    if(hash==NULL){
         printf("Erreur de hashage \n");
         close(fd)
         return NULL;
    }
    char *ch = hashToFile(hash) ;
      if(ch==NULL){
            printf("Erreur de hashage \n");
            close(fd)
            return NULL;
      }
    strcat(ch,".c");
    cp(ch,fname);
    close(fd);
    free(ch);
    return hash ;
}
