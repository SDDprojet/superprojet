
#define SIZE 20

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

typedef struct cell{
char* data ;
struct cell* next ;
}Cell ;

typedef Cell* List ;
int hashFile(char* source, char* dest){
    char buffer[256];
    sprintf(buffer,"cat %s | sha256sum >>%s",source,dest);
    system(buffer) ;
    return 0;
}
char* sha256file(char* file){
    static char template[] ="/tmp/myfileXXXXXX" ;
    char fname[1000];
    char line[256];
    strcpy(fname,template);
    int fd=mkstemp(fname) ;
    hashFile(file,fname);
    char* buffer;
    buffer=malloc(sizeof(char)*256);
    FILE* f = fopen(fname,"r");
    fgets(line,256,f); 
    sscanf(line,"%s",buffer);
    fclose(f);
    

    return buffer;
    

}
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
void freeCell(Cell* c){
    while(c != NULL){
        Cell* next=c->next;
        free(c->data);
        free(c);
        c = next;
    }
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
        printf("Erreur d'ouverture du fichierftol  %s\n",path);
        return NULL;
    }
    char buffer[256];
    fgets(buffer,256, f);
    List * L = stol(buffer);
    fclose(f);
    return L;   
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
struct stat st = {0};
int file_exists (char *file){ 
    struct stat buffer;
    return (stat(file, &buffer) == 0);
}

void cp(char *to, char *from){
    if(file_exists(to)==0){
        printf("le fichier %s n'existe pas cp\n",to );
        return ;
    }
    FILE * source = fopen(from, "r");
    if(source ==NULL){
        printf("Erreur d'ouverture du fichier cp %s\n",from);
        return;
    }
    FILE * dest = fopen(to, "w");
    if(dest ==NULL){
        printf("Erreur d'ouverture du fichier cp %s\n",to);
        fclose(source);
        return;
    }
    char buffer[256];
    while(fgets(buffer,256,source)!=NULL){
        fprintf(dest,"%s",buffer);
    }
    fclose(source);
    fclose(dest);

}

char* hashToPath2(char* hash){
    char* res = malloc(strlen(hash)+1);
    strcpy(res,hash);
    sprintf(res, "%c%c/%s", hash[0],hash[1],hash+2);
    return res;
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
    sprintf(command1, "mkdir -p %c%c",chemin[0],chemin[1]);//jai ajouter -p pour eviter les erreurs si le repertoire existe deja 
    system(command1);
    cp(chemin,file);
}
void blobFile2(char* file){
    char* hash = sha256file(file) ;
    printf("hash = %s \n",hash);
    char* ch2 = strdup(hash) ;
    printf("%s \n ",ch2);
    ch2[2] = '\0' ;
    printf("%s \n ",ch2);
    if (!file_exists(ch2)){
       char buff[100];
       sprintf(buff,"mkdir %s",ch2);
       system(buff);
    }
    char* ch=hashToPath(hash);
    cp(ch,file);
 }

int size_list(List l) {
    int cpt = 0;
    while (l != NULL) {
        cpt++;
        l = l->next;
    }
    return cpt;
}
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
    printf("freeeeeeeee");

    for (int i = 0; i < wt->size; i++) {
        free(wt->tab[i].name);
        free(wt->tab[i].hash);
    }
    free(wt->tab);
    free(wt);
    printf("finnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
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
 mkdir (ch2) ;
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

char* concat_paths(char* c1, char* c2){
    char* ch = malloc(sizeof(char)*strlen(c1)+strlen(c2)+2);
    sprintf(ch,"%s/%s",c1,c2);
	return ch;
}


int isFile(const char *path){
    struct stat path_stat;
    if (file_exists((char*)path) == 1){
        stat(path,&path_stat); 
        return S_ISREG(path_stat.st_mode);   //1 si file 0 si pas file ou n'existe pas
    }
    return -1;
}


char * saveWorkTree(WorkTree *wt,char * path){ //La fonction sauvegarde les fichiers présents dans le WorkTree, dont la racine est àpath
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
        absPath = concat_paths(path,wt->tab[i].name);
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
int isWorkTree(char* hash) {
    char* path = hashToPath(hash);
    if (file_exists(path)) {
        // If the file exists, check if it is a directory or not
        struct stat buffer;
        if (stat(path, &buffer) == 0) {
            if (S_ISDIR(buffer.st_mode)) {
                // If it is a directory, return 1 to indicate that it is a work tree
                free(path);
                return 1;
            } else {
                // If it is a regular file, return 0 to indicate that it is not a work tree
                free(path);
                return 0;
            }
        } else {
            // If there was an error in stat(), return -1 to indicate failure
            free(path);
            return -1;
        }
    } else {
        // If the file does not exist, return -1 to indicate failure
        free(path);
        return -1;
    }
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
		char* absPath= concat_paths(path,wt->tab[i].name);
        char* copyPath=hashToPath(wt->tab[i].hash);
        char* hash=wt->tab[i].hash;
        int status_Wt= isWorkTree(wt->tab[i].hash);
        if(status_Wt==-1){
            printf("Le fichier %s n'existe pas à l'endroit indiqué (peut-être une erreur du paramètre path ?) -> restoreWorkTree\n",absPath);
            exit(EXIT_FAILURE);
        }
        if(status_Wt==0){
            cp(absPath,copyPath);
            setmode(wt->tab[i].mode,absPath);
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


int main(){
    // Création d'un WorkTree
    WorkTree* wt = initWorkTree();
    appendWorkTree(wt, "fichier1", "hash1", 777);
    appendWorkTree(wt, "dossier1", "hash2", 777);
    appendWorkTree(wt, "fichier2", "hash3", 777);

    // Appel à saveWorkTree pour sauvegarder le WorkTree
    char* path = ".";
    char* blob = saveWorkTree(wt, path);
    printf("WorkTree sauvegardé dans le blob : %s\n", blob);

    // Appel à restoreWorkTree pour restaurer le WorkTree
    WorkTree* wt_restored = initWorkTree();
    restoreWorkTree(wt_restored, path);
    printf("WorkTree restauré :\n");

    // Libération de la mémoire
    free(blob);
    freeWorkTree(wt);
    freeWorkTree(wt_restored);

    return 0;
}