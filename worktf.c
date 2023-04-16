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

	char* begin = ch;
	char* end = strchr(ch,'\n');
	int i;
	i = sscanf(begin,"%s\t%s\t%s",name, hash, mode);
	if((end == NULL) && (i != 3)){
		return NULL;	
	} 

	WorkTree* wt = initWorkTree();

	char* content = (char*)malloc(sizeof(char)*SIZE);
	int size_content;

	while(end != NULL){
		size_content = end - begin;
		content = strncpy(content,begin,size_content);
		content[size_content+1] = '\0';
		i = sscanf(content,"%s\t%s\t%s",name, hash, mode);
		if(i == 3){
			appendWorkTree(wt,name, hash, atoi(mode));

		}
		begin = end+1;
		while(begin[0]=='\n'){
			begin ++;
		}
		end = strchr(begin,'\n');
	}
	content = strcpy(content,begin);
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
    printf("-free worktree-");

    for (int i = 0; i < wt->size; i++) {
        free(wt->tab[i].name);
        free(wt->tab[i].hash);
    }
    free(wt->tab);
    free(wt);
    printf("fin de freeWroktree");
}

int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode) {
    int index = inWorkTree(wt, name); // Vérifie si le fichier ou répertoire est déjà présent

    if (index == -1) { // Si le fichier ou répertoire n'existe pas
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
	char buffer[100];
	while(fgets(buffer,100,f) != NULL){
        buffer[strcspn(buffer, "\n")] = '\0';
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

int isFile(const char* name){
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
}

char* saveWorkTree(WorkTree* wt, char* path){
    int i =0;
    char*  hash;
    char* blobHash;

    while(i < wt->size){
        if(isFile(conct(path,wt->tab[i].name))== 1){
            blobFile(conct(path,wt->tab[i].name));
            hash=sha256file(conct(path,wt->tab[i].name));
            wt->tab[i].hash = strdup(hash);
            wt->tab[i].mode = getChmod(conct(path,wt->tab[i].name));
            free(hash);
        }else{
            WorkTree* newWT = initWorkTree();
            List* m = listdir(conct(path,wt->tab[i].name));
            Cell* ptr = m;
            while(ptr != NULL){
                if(strncmp(ptr->data,".",1) != 0){
					appendWorkTree(newWT,ptr->data,"lalal",0);
				}
				ptr=ptr->next;
            }
            hash=saveWorkTree(newWT,conct(path,wt->tab[i].name));
            wt->tab[i].hash =strdup(hash);
			wt->tab[i].mode = getChmod(conct(path,wt->tab[i].name));
            free(hash);
        }
        i++;
    }
    return blobWorkTree(wt);
}

char * saveWorkTree2(WorkTree *wt,char * path){ //La fonction sauvegarde les fichiers présents dans le WorkTree, dont la racine est àpath
    if (wt == NULL){ //Teste si le WorkTree est NULL
        printf("Le WorkTree est NULL -> saveWorkTree\n");
        exit(EXIT_FAILURE);
    }
    if (path == NULL){ //Teste si le path est NULL
        printf("Le path est NULL -> saveWorkTree\n");
        exit(EXIT_FAILURE);
    }
    char * absPath;
    for(int i=0;i<wt->n;i++){ //Parcours des WorkFile présents
        //On récupère le path du WorkFile correspondant
        printf("i = %d \n ",i);
        absPath = conct(path,wt->tab[i].name);
        //On stocke le statut du fichier pour faire nos cas
        int etat_file = isFile(absPath);
        if (etat_file == -1){ //Teste si le fichier n'existe pas
            printf("Le fichier %s n'existe pas à l'endroit indiqué (peut-être une erreur du paramètre path ?) -> saveWorkTree\n",absPath);
            exit(EXIT_FAILURE);
        }
        if (etat_file == 1){ //Teste si le WorkFile est un fichier
            blobFile(absPath);
            wt->tab[i].hash = sha256file(absPath);
            wt->tab[i].mode = getChmod(absPath);
        }
        if (etat_file == 0){ //Teste si le WorkFile est un dossier
            WorkTree *wt2 = initWorkTree();
            //On récupère l'ensemble des fichiers et dossiers présents dans le dossier
            List * L = listdir(absPath);

            for(Cell * ptr = *L;ptr !=NULL ; ptr = ptr->next){ //On construit notre WorkTree
                if(ptr->data[0]=='.'){ //Ignore les fichiers commençant par .
                    continue;
                }
                char buff[1000];
                strcpy(buff,ptr->data);
                appendWorkTree(wt2,buff,NULL,777);
            }
            freeList(L);
            wt->tab[i].hash = saveWorkTree(wt2,absPath);
            wt->tab[i].mode = getChmod(absPath);
            freeWorkTree(wt2);
        }
    free(absPath);
    }
    return blobWorkTree(wt);
}


int isFile2(const char* name){
    DIR* directory = opendir(name);

    if(directory != NULL){
        return 0;//si c'est un répertoire 
    }

    if(errno == ENOTDIR){
        return 1;//si c'est un fichier
    }

    return -1;//si le fichier ou le directory n'existe pas
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

void restoreWorkTree(WorkTree* wt, char* path){
	if(wt == NULL ){
        printf("wt est null rwt");
		exit(EXIT_FAILURE);
	}
    if(path==NULL){
        printf("path est null rwt");
        exit(EXIT_FAILURE);
    }
	for (int i = 0; i < wt->n; i++){
		char* absPath= conct(path,wt->tab[i].name);
        char* copyPath=hashToPath(wt->tab[i].hash);
        char* hash=wt->tab[i].hash;
        int status_Wt= isWorkTree(wt->tab[i].hash);
        if(status_Wt==-1){
            printf("Le fichier %s n'existe pas à l'endroit indiqué (peut-être une erreur du paramètre path ?) -> restoreWorkTree\n",absPath);
            exit(EXIT_FAILURE);
        }
        if(status_Wt==0){
            cp(absPath,copyPath);
            setMode(wt->tab[i].mode,absPath);
        }
        if(status_Wt==1){
            copyPath=realloc(copyPath,strlen(copyPath)+3);
            strcat(copyPath,".t");
            strcat(copyPath,"\0");
            WorkTree* wt2=ftwt(copyPath);
            restoreWorkTree(wt2,absPath);
            setMode(getChmod(copyPath),absPath);
            freeWorkTree(wt2);
        }
        free(absPath);
        free(copyPath);

	}
}




