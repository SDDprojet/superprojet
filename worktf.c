#include "worktf.h"
#include "filehash.h"
#include "liste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>
#include <errno.h>

#define SIZE 20

int getChmod(const char * path) {
    struct stat ret;
    if (stat(path,&ret) == -1){
        return -1;
    }   
    return (ret.st_mode & S_IRUSR) | (ret.st_mode & S_IWUSR) |(ret.st_mode & S_IXUSR ) | /*owner*/
        (ret.st_mode & S_IRGRP) |(ret.st_mode & S_IWGRP) |(ret.st_mode & S_IXGRP ) | /*group*/
        (ret.st_mode & S_IROTH) |(ret.st_mode & S_IWOTH) | (ret.st_mode & S_IXOTH ); /*other*/
}


void setMode(int mode , char* path){
    char buff[100];
    sprintf(buff,"chmod %d %s",mode,path);
    system(buff);
}

WorkFile* createWorkFile(char* name) {
    WorkFile* wf = (WorkFile*)malloc(sizeof(WorkFile));
    wf->name = strdup(name);
    wf->hash = NULL;
    wf->mode = 0;
    return wf;
}
char* wfts(WorkFile* wf) {
    char* ch = malloc(1000*sizeof(char)) ;
    sprintf( ch , "%s \t %s \t %d" , wf->name , wf->hash , wf->mode ) ;
    return ch ;
}

WorkFile* stwf(char* ch) {
    char name[100];
    char hash[100];
    int mode;
    sscanf(ch,"%s\t%s\t%d",name,hash,&mode);
    WorkFile* wf = createWorkFile(name);
    wf->hash = strdup(hash);
    wf->mode = mode;
    return wf;
}

WorkTree* initWorkTree(){
    WorkTree* wt = (WorkTree*)malloc(sizeof(WorkTree));
    wt->tab = (WorkFile*)malloc(SIZE*sizeof(WorkFile));
    wt->size = SIZE;
    wt->n = 0;
    return wt;
}
int inWorkTree(WorkTree* wt, char* name) {
    int i;
    for (i = 0; i < wt->n; i++) {
        if (strcmp(wt->tab[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode) {
    int index = inWorkTree(wt, name); // Vérifie si le fichier ou répertoire est déjà présent

    if (index == -1) { // Si le fichier ou répertoire n'existe pas
        if(wt->size == wt->n){
            printf("Le WorkTree est complet\n");
            return 1;
        }else{// Crée une nouvelle entrée dans le WorkTree
        WorkFile* wf = createWorkFile(name);
        wf->hash = strdup(hash);
        wf->mode = mode;
        wt->tab[wt->n] = *wf;
        wt->n++;
        return 0;
        }
    }return 1;
}
char* wtts(WorkTree* wt){
    char * chaine = malloc(256*sizeof(char));
    chaine[0] = '\0';
    for (int i = 0; i < wt->n; i++){
        strcat(chaine,wfts(&(wt->tab[i])));
        if(i != wt->n-1){ 
            strcat(chaine,"\n");
        }
    }
    return chaine ;
}

int wttf(WorkTree* wt, char* file){
    char* chaine = wtts(wt);
    FILE* f = fopen(file,"w");
    if (f==NULL){
    	printf("Erreur ouverture fichier\n");
        return 1;;
    }
    fprintf(f,"%s",chaine);
	fclose(f);
	free(chaine);
    return 0;
}

WorkTree* ftwt(char* file){
	FILE* f = fopen(file,"r");
	if (f==NULL){
    	printf("Erreur ouverture fichier\n");
        exit(EXIT_FAILURE);
    }
    WorkTree* wt = initWorkTree();
    WorkFile* wf;
	char buffer[100];
	while(fgets(buffer,100,f) != NULL){
        wf = stwf(buffer);
        appendWorkTree(wt,wf->name,wf->hash,wf->mode);
    }
	fclose(f);
	return wt;
}
char* hashToFile (char* hash ) {
 char* ch2 = strdup(hash) ;
 struct stat st;
 ch2[2] = '\0' ;
 if(stat(ch2,&st) == -1) {
 mkdir (ch2,0700) ;
 }
 return hashToPath ( hash ) ;
 }


char* blobWorkTree2(WorkTree* wt){
    static char template[] = "/tmp/myfileXXXXXX";
    char fname[100];
    strcpy(fname,template);
    mkstemp(fname);
    wttf(wt,fname);
    char* hash = sha256file(fname);
    char* chemin = hashToPath(hash);
    strcat(chemin,".t");
    char command1[500];
    sprintf(command1, "mkdir %c%c",chemin[0],chemin[1]);
    system(command1);
    char command2[500];
    sprintf(command2, "cat %s > %s",fname,chemin);
    system(command2);
    return hash;
}

char* conct(char* c1, char* c2){
    char* ch = malloc(sizeof(char)*strlen(c1)+strlen(c2)+2);
    sprintf(ch,"%s/%s",c1,c2);
	return ch;
}

int isFile2(const char* name){
    DIR* dir = opendir(name);
    if (dir != NULL) {
        printf("%s est un répertoire.\n", name);
        closedir(dir);
        return 0;
    }
    else {
        FILE* file = fopen(name, "r");
        if (file != NULL) {
            printf("%s est un fichier.\n", name);
            fclose(file);
            return 1;
        }
        else {
            printf("%s n'est pas un fichier et un répertoire .\n", name);
            return -1;
        }
    }

    return -1;
}

char* saveWorkTree(WorkTree* wt, char* path){
    int i =0;

    while(i < wt->size){
        if(isFile(conct(path,wt->tab[i].name))== 1){
            blobFile(conct(path,wt->tab[i].name));
            wt->tab[i].hash = sha256file(conct(path,wt->tab[i].name));
            wt->tab[i].mode = getChmod(conct(path,wt->tab[i].name));

        }else{
            WorkTree* newWT = initWorkTree();
            List* m = listdir(conct(path,wt->tab[i].name));
            Cell *ptr = m;
            while(ptr != NULL){
                if(strncmp(ptr->data,".",1) != 0){
					appendWorkTree(newWT,ptr->data,"lalal",0);
				}
				ptr=ptr->next;
            }
            wt->tab[i].hash = saveWorkTree(newWT,conct(path,wt->tab[i].name));
			wt->tab[i].mode = getChmod(conct(path,wt->tab[i].name));

        }
        i++;
    }
    return blobWorkTree2(wt);
}


int isFile(const char* name){
    DIR* directory = opendir(name);

    if(directory != NULL){
        return 0;//si c'est un répertoire 
    }

    if(errno == ENOTDIR){
        return 1;//si c'est un fichier
    }

    return -1;//si le fichier ou le directory n'existe pas
}


void restoreWorkTree(WorkTree* wt, char* path){
	if(wt == NULL || wt->n<=0 ){ // Si pas d'élément dans tab
		return ;
	}

	WorkFile* wf_current = NULL;
	int i;
	for (i = 0; i < wt->n; i++){
		wf_current = &(wt->tab[i]);
		int nb_char = strlen(wf_current->hash);
		if(wf_current->hash[nb_char-2] != '.'){// S'il sagit d'un fichier
			cp(conct(path,wf_current->name),hashToPath(wf_current->hash));
			setMode(wf_current->mode,conct(path,wf_current->name));

		}else{
			WorkTree* newWT = ftwt(hashToPath(wf_current->hash));
			restoreWorkTree(newWT,conct(path,wf_current->name));
		}

	}
}

