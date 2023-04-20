#include "worktf.h"
#include "commit.h"
#include "filehash.h"
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
#include <stdbool.h>
#include <errno.h>

List* initList(){
    List* l=(List*)(malloc(sizeof(List)));
    *l=NULL;
    return l;
}

Cell* buildCell(char* ch){
    Cell* c = (Cell*)(malloc(sizeof(Cell)));
    if(c==NULL){
        printf("Erreur d'allocation mémoire : bc");
        return NULL;
    }
    c->data = strdup(ch);
    if(c->data==NULL){
        printf("Erreur d'allocation mémoire :bc");
        return NULL;
    }
    c->next = NULL;

    return c;
}
void freeCell(Cell *c){
    if(c == NULL){
        return;
    }

    if(c->data) free(c->data);
    free(c);
}
void insertFirst(List *L,Cell* C){
    C->next=*L;
    *L=C;
    
}

char* ctos(Cell* c){
    return c->data;
}


char* ltos(List* L){
    char * res = malloc(1000);
    if(res==NULL){
        printf("Erreur d'allocation mémoire ltos");
        return NULL;
    }
    res[0]='\0';
    Cell * tmp = *L;
    while(tmp){
        strcat(res, ctos(tmp));
        strcat(res, "|");
        tmp = tmp->next;
    }
    return res;
}
Cell* listGet2(List* l, int i){
	while(*l){
		if(i==1){
			return *l;
		}
		*l=(*l)->next;
		i--;
	}
	return NULL;
}
Cell* listGet(List* l, int i){
    int index = 1; 
    Cell* tmp = *l;
    
    while(tmp){
        if(index == i){
            return tmp;
        }
        tmp = tmp->next;
        index++;
    }
    
    return NULL;
}


Cell* searchList(List* L, char* str){
    Cell * tete = *L;
    while(tete){
        if(strcmp(tete->data,str)==0){
            return tete;
        }
        tete = tete->next;
    }
    freeCell(tete);
    return NULL;
}


List* listdir(char* root_dir){
    DIR* dp = opendir(root_dir) ;
    List* L = initList();
    struct dirent *ep ;
    if(dp != NULL){
        while ((ep = readdir(dp)) != NULL){   
            Cell* c1 = buildCell(ep->d_name);
            insertFirst(L,c1);
        }
        closedir(dp);
    }
    return L;
}

void insertLast(List *L, Cell *C){
    if (*L == NULL){
        *L = C;
        return;
    }
    List l = (*L);
    while(l->next){
        l=l->next;
    } 
    C->next = l->next;
    l->next = C;
}
List* stol(char* s){
    List* L = initList();
    if (s == NULL){
        return L;
    }
    char data[100];
    int r = 0;
    int i = 0;
    while (s[r]!='\0'){
        if (s[r] == '|'){
            data[i] = '\0'; 
            insertLast(L, buildCell(data));
            i = 0;
        }
        else {
            data[i] = s[r];
            i++;
        }
        r++;
    }
    data[i] = '\0'; 
    insertLast(L, buildCell(data));
    return L;
}

void ltof(List* L, char* path){
    FILE * f = fopen(path, "w");
    if(f==NULL){
        printf("Erreur d'ouverture du fichier ltof %s\n",path);
        return;
    }
    char * chaine = ltos(L);
    fprintf(f, "%s\n",chaine);
    fclose(f);
}

List* ftol(char* path){
    FILE * f = fopen(path, "r");
    if(f==NULL){
        printf("Erreur d'ouverture du fichier ftol  %s\n",path);
        return NULL;
    }
    char buffer[256];
    fgets(buffer,256, f);
    List * L = stol(buffer);
    fclose(f);
    return L;   
}


struct stat st = {0};

_Bool file_exists( char * file ) {
    struct stat buffer ;
    return (stat(file,&buffer) == 0);
}
void cp(char *to,char *from){

    if(to == NULL || from == NULL){
        printf("erreurs : la chaine to/from est vide \n");
    }
    char *cur = strdup(to);
    char *file = strrchr(cur, '/');
    if(file){
        file[0] = '\0';
    }
    char *ch = strtok(cur, "/");
    char cur_dir[256] = ".";
    while(ch != NULL){
        strcat(cur_dir, "/");
        strcat(cur_dir, ch);
        struct stat sb;
        if (stat(cur_dir, &sb) == 0) { // si dossier ou fichier du meme nom existe
            printf(" '%s' est le nom déjà pris par un fichier ou un dossier.\n",cur_dir);     
        } else if (mkdir(cur_dir, 0700) != 0) {
            free(cur);
            return;
        }
    ch = strtok(NULL, "/");
    }
    free(cur);
    FILE *dest = fopen(to, "w");
    if (dest == NULL){
        return;
    }
    FILE *src = fopen(from, "r");
    if(src == NULL){
        fclose(dest);
        return;
    }
    char buf[256];
    while (fgets(buf, 256, src))
        fputs(buf, dest);
    fclose(dest);
    fclose(src);
}

void freeList(List* L){
    Cell* current = *L;
    Cell* next;
    while (current != NULL){
        next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
    *L = NULL; 
}


char *filePath( char *hash){
    char* path = malloc(sizeof(char) * 256);

    if(path == NULL || hash == NULL){
        printf("path est Null ");
        return NULL;
    }

    path[0] = '\0';

    char *hashP = hashToPath(hash);

    if(hash == NULL){
        printf("erreur : hashToPath a la valeur NULL avec %s", hash);
        free(path);
        return NULL;
    }

    strcat(path, ".");
    strcat(path, "/");
    strcat(path, hashP);

    free(hashP);

    return (hashP);
}

char* hashToPath(char* hash){
    char* res = malloc(strlen(hash)+3);
    if(res == NULL){
        printf("Erreur d'allocation de mémoire htp\n");
        return NULL;
    }
    
    sprintf(res, "%c%c/%s", hash[0],hash[1],hash+2);
    return res;
}

void blobFile(char* file){
    if(file_exists(file)==0){
        printf("Erreur : le fichier %s n'existe pas bf\n", file);
        return  ;
    }
    char* hash = sha256file(file);
    char* chemin = hashToPath(hash);
    char command1[500];
    sprintf(command1, "mkdir -p %c%c",chemin[0],chemin[1]); //-p si le répertoire existe déjà
    system(command1);
    cp(chemin,file);
}

int size_list(List l) {
    int cpt = 0;
    while (l != NULL) {
        cpt++;
        l = l->next;
    }
    return cpt;
}
