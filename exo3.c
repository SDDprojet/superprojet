#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "ListLC.h"

int hashFile(char* source, char* dest){
    char* commande = malloc(256);
    sprintf(commande,"cat %s | sha256sum > %s ", source, dest);
    return system(commande);
}

char* sha256file(char* file){
    static char template[] = "fileXXXXXX" ;
    char fname [1000];
    strcpy(fname,template);
    int fd = mkstemp(fname);
    hashFile(file, fname);
    char * res = malloc(256);
    dup2(fd,0);
    scanf("%s", res);

    char* commande = malloc(256);
    sprintf(commande, "rm -f %s", fname);
    system(commande);
    free(commande);
    return res;
}

List* initList(){
    List * l = malloc(sizeof(List));
    *l = NULL;
    return l;
}

Cell* buildCell(char* ch){
    Cell * c = malloc(sizeof(Cell));
    c->data = strdup(ch);
    c->next = NULL;
    return c;
}

void insertFirst(List *L, Cell* C){
    if(*L==NULL){
        *L = C;
    }else{
        C->next = *L;
        *L = C;
    }
}

char* ctos(Cell* c){
    return c->data;
}

char* ltos(List* L){
    char * res = malloc(1000);
    Cell * tmp = *L;
    while(tmp){
        strcat(res, ctos(tmp));
        strcat(res, "|");
        tmp = tmp->next;
    }
    return res;
}

Cell* listGet(List* L, int i){
    Cell * tmp = *L;
    if(L==NULL){
        return NULL;
    }
    int j;
    for(j = 0; j<i && tmp->next!=NULL; j++){
        tmp = tmp->next;
    }
    if(j==i) return tmp;
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
    return NULL;
}

List* stol(char* s){
    List * L = initList();
    int debut = 0, i=0, cpt = 0;
    char *tmp = malloc(256);
    while(s[i]!= '\0'){
        printf("la valeur de i = %d \n",i);
        printf("la valeur de s = %d \n",strlen(s));
        if(s[i]!='|'){
            cpt++;
        }else if (i<strlen(s)){
            printf("on est dans la boule");
            tmp = strncpy(tmp, s+debut, cpt);
            Cell * C = buildCell(tmp);
            insertFirst(L,C);

            debut = i+1;
            cpt = 0;
        }
        i++;
    }
    return L;
}



void affichageList(List *L){
    Cell * C = *L;
    while(C){
        printf("%s\n",ctos(C));
        C = C -> next;
    }
    return;
}

void ltof(List* L, char* path){
    FILE * f = fopen(path, "w");
    char * chaine = ltos(L);
    fprintf(f, "%s\n",chaine);
    fclose(f);
}

List* ftol(char* path){
    FILE * f = fopen(path, "r");
    char buffer[256];
    fgets(buffer,256, f);
    List * L = stol(buffer);
    return L;   
}

List * listdir(char * root_dir){
    List * L = initList();
    DIR * dp = opendir(root_dir);
    struct dirent * ep;
    if(dp != NULL){
        while ((ep = readdir(dp))!= NULL){
            Cell * c = buildCell(ep->d_name);
            insertFirst(L,c);
        }
    }
    return L;
}


int file_exists(char * file){
    char * res = malloc(256);
    char* commande = malloc(256);

    char tmp[] = "fileXXXXXX" ;
    int fd = mkstemp(tmp);

    sprintf(commande, "pwd > %s", tmp);
    system(commande);

    dup2(fd,0);
    scanf("%s ", res);

    printf("%s\n",res);
    char* commande2 = malloc(256);
    sprintf(commande2, "rm -f %s", tmp);
    system(commande2);
    List * l = listdir(res);
    free(commande);
    free(commande2);
    if (searchList(l, file)==NULL){
        return 0;
    }
    return 1;
}


void cp(char *to, char *from){
    FILE * source = fopen(from, "r");
    if(source==NULL){
        printf("Erreur a l'ouverture - la source n'existe pas\n");
        return ;
    }
    FILE * dest = fopen(to, "w");
    char buffer[256];
    while(fgets(buffer,256,source)!=NULL){
        fprintf(dest,"%s",buffer);
    }
    fclose(source);
    fclose(dest);
}

char * hashToPath(char * hash){
    char * res = malloc(strlen(hash)+1);
    sprintf(res, "%c%c/%s", hash[0],hash[1],hash+2);
    return res;
}

//a refaire
void blobFile(char * file){
    if(file_exists(file)==0){
        printf("Erreur : le fichier %s n'existe pas\n", file);
        return ;
    }
    char * hashPath = hashToPath(sha256file(file));
    char* commande = malloc(256);
    sprintf(commande,"mkdir %c%c", hashPath[0],hashPath[1]);
    cp(file, hashPath+2);
}
