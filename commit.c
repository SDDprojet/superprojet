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
    return k ;
}

void freeKeyVal(kvp* kv){
    free(kv->key);
    free(kv->value);
    free(kv);
}

char* kvts(kvp* k){ //deja testé
	if(k == NULL){
		return NULL;
	}
	char *chaine = (char *)malloc(sizeof(char)*255);
	sprintf(chaine,"%s :%s",k->key,k->value);
	return chaine;
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
    c->size = SIZE;
    c->T = malloc(SIZE*sizeof(kvp*));
    for(int i = 0; i< c->size ; i++ ){
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
    unsigned long p = hash(key)%c->size ;
    while(c->T[p] != NULL ) {
        p =(p +1)%c-> size ; //probing lineaire
    }
    c->T[p] =createKeyVal(key,value);
    c->n++;
}
Commit* createCommit(char* hash){ //deja testé
    Commit* c =initCommit();
    commitSet(c,"tree",hash);
    return c;
}

char* commitGet(Commit* c,char* key){ //deja testé
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

char* cts(Commit* c){ //deja testé
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


Commit* stc(char* ch) {
    Commit* c = initCommit(); // Supposons que la fonction initCommit() soit définie et renvoie un pointeur vers Commit
    char buffer[256]; // Lecture de chaque ligne
    int i = 0;
    int r = 0;
    if (ch == NULL) {
        return c;
    }
    kvp* ptr = NULL; // Déclaration de la variable ptr
    while (ch[r] != '\0') {
        if (ch[r] == '\n') {
            buffer[i] = '\0';
            ptr = stkv(buffer); // Enregistre kv dans la variable ptr
            commitSet(c, ptr->key, ptr->value); // Sauvegarde ce kv dans commit
            freeKeyVal(ptr); // Supposons que la fonction freeKeyVal() est définie et libère la mémoire allouée par ptr
            i = 0; // Réinitialise i pour la prochaine ligne
        } else {
            buffer[i] = ch[r];
            i++;
        }
        r++;
    }
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
    char chaine = malloc(sizeof(char)*strlen(buffer));
    kvp* ptr = malloc(sizeof(kvp)); // Déclaration et initialisation de la variable ptr
    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        strcat(chaine,buffer);
    }
    Commit* c = stc(chaine);
    fclose(f);
    return c; // Retour du pointeur vers Commit c
}
char * blobCommit ( Commit * c ) {
    char fname[100] = " /tmp/myfileXXXXXX" ;
    int fd =mkstemp(fname);
    ctf(c,fname);
    char *hash = sha256file(fname) ;
    char *ch = hashToFile(hash) ;
    strcat(ch,".c");
    cp(ch,fname);
    return hash ;
}
