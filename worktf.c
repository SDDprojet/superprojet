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
    return (ret.st_mode & S_IRUSR) | (ret.st_mode & S_IWUSR) |(ret.st_mode & S_IXUSR)| /*owner*/
        (ret.st_mode & S_IRGRP) |(ret.st_mode & S_IWGRP) |(ret.st_mode & S_IXGRP)| /*group*/
        (ret.st_mode & S_IROTH) |(ret.st_mode & S_IWOTH) | (ret.st_mode & S_IXOTH ); /*other*/
}

void setMode(int mode , char* path){
    char buff[100];
    sprintf(buff,"chmod %d %s",mode,path);
    system(buff);
}

WorkFile* createWorkFile(char* name) {
    WorkFile* wf = (WorkFile*)malloc(sizeof(WorkFile));
    if(wf==NULL){
        printf("Erreur d'allocation mémoire\n");
        return NULL;
    }
    wf->name = strdup(name);
    if(wf->name==NULL){
        printf("Erreur d'allocation mémoire\n");
        free(wf);
        return NULL;
    }
    wf->hash = NULL;
    wf->mode = 0;
    return wf;
}
char* wfts(WorkFile* wf) {
    char* ch = malloc(1000*sizeof(char)) ;
    if(ch==NULL){
        printf("Erreur d'allocation mémoire\n");
        return NULL;
    }
    sprintf( ch , "%s \t %s \t %d" , wf->name , wf->hash , wf->mode ) ;
    return ch ;
}

WorkFile* stwf(char* ch) {
    char name[100];
    char hash[100];
    int mode;
    if(sscanf(ch,"%s\t%s\t%d",name,hash,&mode)!=3){
        printf("Erreur de lecture : stwf\n");
        return NULL;
    }
    WorkFile* wf = createWorkFile(name);
    wf->hash = strdup(hash);
    wf->mode = mode;
    return wf;
}

WorkTree* stwt(char* ch){

	char name[255];
	char hash[255];
	char mode[255];

	char* debut = ch;
	char* fin = strchr(ch,'\n');
	int i;
	i = sscanf(debut,"%s\t%s\t%s",name, hash, mode);
	if((fin == NULL) && (i != 3)){
		return NULL;	
	} 

	WorkTree* wt = initWorkTree();

	char* content = (char*)malloc(sizeof(char)*SIZE);
	int size;

	while(fin != NULL){
		size = fin - debut;
		content = strncpy(content,debut,size);
		content[size+1] = '\0';
		i = sscanf(content,"%s\t%s\t%s",name, hash, mode);
		if(i == 3){
			appendWorkTree(wt,name, hash, atoi(mode));

		}
		debut = fin+1;
		while(debut[0]=='\n'){
			debut ++;
		}
		fin = strchr(debut,'\n');
	}
	content = strcpy(content,debut);
	i = sscanf(content,"%s\t%s\t%s",name, hash, mode);
	if(i == 3){
		appendWorkTree(wt,name, hash, atoi(mode));
	}

	return wt;

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

void freeWorkTree(WorkTree* wt) {
    if (wt == NULL) {
        return;
    }

    for (int i = 0; i < wt->size; i++) {
        free(wt->tab[i].name);
        free(wt->tab[i].hash);
    }
    free(wt->tab);
    free(wt);
}

int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode) {
    //présence de fichier/répertoire
    int id = inWorkTree(wt, name); 

    if (id == -1) { // Si id n'existe pas
        if(wt->n == wt->size){
            printf("Le WorkTree est complet\n");
            return 1;
        } else {// Crée une nouvelle entrée dans le WorkTree
            WorkFile* wf = createWorkFile(name);
            wf->hash = strdup(hash);
            wf->mode = mode;
            wt->tab[wt->n] = *wf;
            wt->n++;
            return 0;
        }
    }
    return 1;
}
char* wtts(WorkTree* wt){
    char * chaine = malloc(256*sizeof(char));
    chaine[0] = '\0';
    for (int i = 0; i < wt->n; i++){
        char* res=wfts(&(wt->tab[i]));
        strcat(chaine,res);
        free(res);
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
        free(chaine);
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
	char buffer[256];
	while(fgets(buffer,256,f) != NULL){
        buffer[strcspn(buffer, "\n")] = '\0';
        wf = stwf(buffer);
        appendWorkTree(wt,wf->name,wf->hash,wf->mode);
    }
	fclose(f);
	return wt;
}
char* hashToFile(char* hash ){
    char* ch2 = strdup(hash) ;
    struct stat st;
    ch2[2] = '\0' ;
    if(stat(ch2,&st) == -1) {
        mkdir(ch2,0700) ;
    }
    return hashToPath(hash);
}

char* workTreeToPath(const char* hash){
  if(hash == NULL){
    printf("hash est null");
    return NULL;
  }

  char *hash_path = hashToPath(hash);

  if(hash_path == NULL){
    printf("hashToPath est NULL", hash);
    return NULL;
  }

  char *path = malloc(sizeof(char) * 256);
  memset(path, 0, 256); 

  strcat(path, ".");
  strcat(path, "/");
  strcat(path, hash_path);
  strcat(path, ".t");

  free(hash_path);

  return path;
}


char* blobWorkTree(WorkTree* wt){
    static char template[] = "/tmp/myfileXXXXXX";
    char fname[100];
    strcpy(fname,template);
    mkstemp(fname);
    wttf(wt,fname);
    char* hash = sha256file(fname);
    char* chemin = hashToPath(hash);
    strcat(chemin,".t");
    char command1[500];
    sprintf(command1, "mkdir -p %c%c",chemin[0],chemin[1]);
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




char* saveWorkTree(WorkTree* wt, char* path){
    for (int i = 0; i < wt->n; i++){
        char *chaine = malloc(sizeof(char) * (strlen(path) + strlen(wt->tab[i].name) + 2));

        memset(chaine, 0, sizeof(char) * (strlen(path) + strlen(wt->tab[i].name) + 2));

        strcat(chaine, path);
        if(!(wt->tab[i].name[0] && wt->tab[i].name[0] == '/')) strcat(chaine, "/");
            strcat(chaine, wt->tab[i].name);

        if (file_exists(chaine)){
            blobFile(chaine);
            wt->tab[i].hash = sha256file(chaine);
            wt->tab[i].mode = getChmod(chaine);
        }
        else{
            WorkTree *wt_rep = initWorkTree();
            List *L = listdir(chaine);
            for (Cell *c = *L; c != NULL; c = c->next){
                if (ctos(c)[0] == '.')
                continue;
                appendWorkTree(wt_rep, ctos(c), NULL, 0);
            }
            wt->tab[i].hash = saveWorkTree(wt_rep, chaine);
            wt->tab[i].mode = getChmod(chaine);
        }
    }
    return blobWorkTree(wt);
}


int isFile(const char* name){
    DIR* directory = opendir(name);

    if(directory != NULL){
        return 0;//si c'est un répertoire 
    }

    if(errno == ENOTDIR){
        return 1;//si c'est un fichier
    }

    return -1;//si c'est ni fichier ni répertoire 
}

int isWorkTree(char * hash){
    if(file_exists(strcat(hashToPath(hash), ".t"))){
       return 1;
    }
    if(file_exists(hashToPath(hash))){
       return 0;
    }
    return -1;
 }

void restoreWorkTree(WorkTree *wt, char *path)
{
    for (int i = 0; i<wt -> n; i++){
      size_t size = strlen(path) + strlen(wt->tab[i].name) + 2;
      char *chemin = malloc(sizeof(char) * size);
      memset(chemin, 0, size);

      strcat(chemin, path);
      strcat(chemin, "/");
      strcat(chemin, wt->tab[i].name);

      char *hash = wt->tab[i].hash;
      if (isWorkTree(hash) == 0)
      { //un fichier
        cp(chemin, filePath(hash));
        setMode(getChmod(filePath(hash)), chemin);
      }
      else if (isWorkTree(hash) == 1)
      { //un repertoire
        WorkTree *nwt = ftwt(workTreeToPath(hash));
        restoreWorkTree(nwt, chemin);
        setMode(getChmod(workTreeToPath(hash)), chemin);
        freeWorkTree(nwt);
      }
      free(chemin);
    }
}   




