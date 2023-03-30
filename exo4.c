#define SIZE 20
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct {
    char* name;
    char* hash;
    int mode;
} WorkFile;
typedef struct{
    WorkFile* tab;
    int size;
    int n;
}WorkTree;

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
    if(wf != NULL){
        int size = strlen(wf->name) + strlen(wf->hash) + 10;
        char* res = (char*) malloc(size * sizeof(char));
        sprintf(res, "%s\t%s\t%d", wf->name, wf->hash, wf->mode);
        return res;
    }
    return NULL;
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
    for (int i = 0; i < SIZE; i++) {
        wt->tab[i].name = NULL;
        wt->tab[i].hash = NULL;
        wt->tab[i].mode = -1;
    }
    return wt;
}
int inWorkTree(WorkTree* wt, char* name) {
    int i;
    for (i = 0; i < wt->size; i++) {
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
        }
        
        else{// Crée une nouvelle entrée dans le WorkTree
        WorkFile* wf = createWorkFile(name);
        wf->hash = strdup(hash);
        wf->mode = mode;
        wt->tab[wt->n] = *wf;
        wt->n++;
        return 0;
        }
    } 
    else { // Si le fichier ou répertoire existe déjà
        // Met à jour l'entrée existante dans le WorkTree
        wt->tab[index].hash = strdup(hash);
        wt->tab[index].mode = mode; // Affecte le nouveau mode
        return 1;
    }

    
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
    sprintf(command1, "mkdir %c%c",chemin[0],chemin[1]);
    system(command1);
    char command2[500];
    sprintf(command2, "cat %s > %s",fname,chemin);
    system(command2);
    return hash;
}


int main(){
    return 0;
}
