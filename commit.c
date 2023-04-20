#include "worktf.h"
#include "filehash.h"
#include "liste.h"
#include "commit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#define SIZE 20


kvp* createKeyVal(char* key,char* val){ //deja testé
    // cree un kvp
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
    //libere l'espace memoire de kv
    if(kv != NULL){
        free(kv->key);
        free(kv->value);
        free(kv);
    }
}

char* kvts(kvp* k){ //deja testé
//transforme de kvp a string
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
//transforme de string a kvp
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
void freeCommit(Commit *c){
//libere l'espace memoire du commit c
  if(c == NULL){
    printf("commit est nul \n");
    return;
  }

  for(int i = 0; i < c->size; i++)
    if(c->T[i] != NULL) freeKeyVal(c->T[i]);

  free(c->T);
  free(c);

  c = NULL;
}
Commit* initCommit(){ //deja testé
//initialise un commit
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
//hash
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
//cree un commit
    if(hash==NULL){
         printf("Erreur : createCommit");
         return NULL;
   }
    Commit* c =initCommit();
    commitSet(c,"tree",hash);
    return c;
}

char* commitGet(Commit* c,char* key){ //deja testé
//retourne le commit qui a comme cle key
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
//transforme de commit a string
    int i = 0;
    int chaine_len = 0;
    int max_len = sizeof(char) * 100 * c->n;
    char *chaine = malloc(max_len*sizeof(char));
    if(chaine != NULL){
        memset(chaine,0,max_len*sizeof(char));

    }
    while (i < c->size) {
        if (c->T[i] != NULL) {
            char *tmp = kvts(c->T[i]);
            if (chaine_len + strlen(tmp) + 1 < max_len) {
                strcat(chaine, tmp);
                strcat(chaine, "\n");
                chaine_len += strlen(tmp) + 1;
            } else {
                printf("Erreur: cts\n");
                break;
            }
            free(tmp);
        }
        i++;
    }
    return chaine;
}


Commit *stc(char *s){ //déjà tester 
//transforme de string a commit
    Commit *c = initCommit();   
    char *token = strtok(s, "\n"); //strtok : séparer les mots avec deuxième param

    while(token){
        kvp* pair = stkv(token);

    commitSet(c, pair->key, pair->value);
    freeKeyVal(pair);
    token = strtok(NULL, "\n");
    }
    free(token);
    return c;  
}


void ctf(Commit *c, char *file){
//transforme un commit en fichier
  FILE *f = fopen(file, "w");
  if(f == NULL){
    printf("probleme d'ouverture de fichier :ctf \n");
    return;
  }

  char *s = cts(c);
  fputs(s, f);
  free(s);
  fclose(f);
}

Commit* ftc(char* file){
	if(file == NULL){
		return NULL;
	}
	FILE* f = fopen(file,"r");
	if(f == NULL){
		if(errno == ENOENT){
			//printf("ftc: Le fichier %s n'existe pas !! \n",file);
			return NULL;
		}else{
			//printf("ftc: autre erreur fopen\n");
			return NULL;
		}
	}
	Commit *c = initCommit();
	char key[255];
	char value[255];
	char buff[255];
	while(fgets(buff,255,f) != NULL){
		if (sscanf(buff,"%s :%s",key,value) == 2){
		commitSet(c,key,value);
		}
	}
	fclose(f);
	return c;
}

char *blobCommit(Commit *c){

// crée unfichier temporaire c.tmp en écrivant le contenu de la structure dans ce fichier à l'aide de la fonction ctf
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
char* commitPath( char* hash){
//renvoi le path du hash
  if(hash == NULL){
    printf( "Tentative de conversion avec un hash null");
    return NULL;
  }

  char *hash_path = hashToPath(hash);

  if(hash_path == NULL){
    printf("hashToPath(\"%s\")  a renvoyé NULL", hash);
    return NULL;
  }

  char *path = malloc(sizeof(char) * 100);
  memset(path, 0, 100); 

  strcat(path, ".tmp");
  strcat(path, "/");
  strcat(path, hash_path);
  strcat(path, ".c");

  free(hash_path);

  return path;
}
