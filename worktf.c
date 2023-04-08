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
 mkdir ( ch2, 0700) ;
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
/*
int isFile(const char* name)
{
    DIR* directory = opendir(name);

    if(directory != NULL)
    {
     closedir(directory);
     return 0;//si c'est un directory
    }

    if(errno == ENOTDIR)
    {
     return 1;//si c'est un fichier
    }

    return -1;//si le fichier ou le directory n'existe pas
}

char * conc(char* char1, char* char2){
	char *  res = malloc(sizeof(char)*strlen(char1)+strlen(char2)+2);
	sprintf(res,"%s/%s",char1,char2);
	return res;
}	
char* saveWorkTree(WorkTree* wt, char* path){
	if(wt == NULL || wt->n<=0 ){ // Si pas d'élément dans tab
		return NULL;
	}
	//system("pwd");
	//printf("path =%s\n",path);
	//printf("DEBUT WT =%s\n",wtts(wt));
	int i;
	printf("n =%d\n",wt->n);
	for(i=0; i<wt->n; i++){
		printf("i =%d\n",i);
		printf("nom courant =%s\n",wt->tab[i].name );
		char* hash=malloc(sizeof(char)*255);
		int mode;
		if (isFile(conc(path,wt->tab[i].name))==1){
			blobFile(conc(path,wt->tab[i].name));
			hash=sha256file(conc(path,wt->tab[i].name));
			mode=getChmod(conc(path,wt->tab[i].name));
			wt->tab[i].mode=mode;
			wt->tab[i].hash=hash;
			printf("name %s \nmode %d \n hash %s \n",wt->tab[i].name,wt->tab[i].mode,wt->tab[i].hash);
		}else{
			WorkTree* newWT=initWorkTree();
			List *L=listdir(conc(path,wt->tab[i].name));
			Cell *ptr = *L;
			int k = 0;
			while(ptr!= NULL){
				if(strncmp(ptr->data,".",1) != 0){
					printf("%s\n",ptr->data);
					appendWorkTree(newWT,ptr->data,"lalal",0);
				}
				ptr=ptr->next;
			}
			wt->tab[i].hash = saveWorkTree(newWT,conc(path,wt->tab[i].name));
			wt->tab[i].mode = getChmod(conc(path,wt->tab[i].name));
		}
	}
		return blobWorkTree(wt) ;
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
			cp(conc(path,wf_current->name),hashToPath(wf_current->hash));
			setMode(wf_current->mode,conc(path,wf_current->name));

		}else{
			WorkTree* newWT = ftwt(hashToPath(wf_current->hash));
			restoreWorkTree(newWT,conc(path,wf_current->name));
		}

	}
}
*/
