#include "branch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#define SIZE 20





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
typedef struct key_value_pair {
 char* key ;
 char* value ;
 }kvp ;

 typedef struct hash_table {
 kvp** T ;
 int n ;
 int size ;
 }HashTable;

 typedef HashTable Commit ;
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

void cp(char *to,char *from)
{
    
    char *curs = strdup(to);
    char *file = strrchr(curs, '/');

    if(file) file[0] = '\0';

    char *token = strtok(curs, "/");


    char current_dir[256] = ".";
    while(token != NULL){
        strcat(current_dir, "/");
        strcat(current_dir, token);

        struct stat sb;
        if (stat(current_dir, &sb) == 0) { // Un dossier ou un fichier du meme nom existe
            printf("Le nom '%s' est déjà pris par un fichier ou un dossier. Code : %d", current_dir, errno);
        
        } else if (mkdir(current_dir,0700) != 0) {
            free(curs);
            return;
        }

    token = strtok(NULL, "/");
    }
    
    free(curs);

    FILE *dest = fopen(to, "w");

    if (dest == NULL){
        return;
    }

    FILE *source = fopen(from, "r");

    if(source == NULL){
        fclose(dest);
        return;
    }

    char buf[256];
    while (fgets(buf, 256, source))
        fputs(buf, dest);

    fclose(dest);
    fclose(source);
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
    sprintf( ch , "%s \t %s \t %d" , wf->name , wf->hash , wf->mode);
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
char* hashToFile (char* hash){
 char* ch2 = strdup(hash) ;
 struct stat st;
 ch2[2] = '\0' ;
 if(stat(ch2,&st) == -1) {
 mkdir(ch2,0700) ;
 }
 return hashToPath(hash);
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


int isFile(const char* name)
{
    DIR* directory = opendir(name);

    if(directory != NULL){
     closedir(directory);
     return 0;
    }

    if(errno == ENOTDIR){
     return 1;
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
        printf("i = %d \n ",i);
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

	char* content = (char*)malloc(sizeof(char)*256);
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



kvp* createKeyVal(char* key,char* val){ //deja testé
    kvp* k = malloc(sizeof(kvp));
    if(k==NULL){
        printf("Erreur d'allocation mémoire : ckv");
        return NULL;
    }
    k->key = strdup(key);
    if(k->key==NULL){
        printf("Erreur d'allocation mémoire : ckv");
        free(k);
        return NULL;
    }
    k->value = strdup(val);
    if(k->value==NULL){
        printf("Erreur d'allocation mémoire : ckv");
        free(k->key);
        free(k);
        return NULL;
    }
    return k ;
}

void freeKeyVal(kvp* kv){
    if(kv!=NULL){
        free(kv->key);
        free(kv->value);
        free(kv);
    }
}

char* kvts(kvp* k){ //deja testé
	if(k == NULL|| k->key == NULL || k->value == NULL){
		return NULL;
	}
	char *chaine = (char *)malloc(sizeof(char)*255);
   if(chaine==NULL){
         printf("Erreur d'allocation mémoire : kvts");
         return NULL;
   }
	snprintf(chaine,255,"%s :%s",k->key,k->value);
	return chaine;
}

kvp* stkv(char* s){ //deja testé
    int r=0;
    char mot1[100];
    char mot2[100];
    int i=0;
    if (strchr(s, ':') == NULL) {
        return NULL;
    }
    while (s[r]!=':' && r<99){
           mot1[r] = s[r];
           r++ ;
    }

    mot1[r]='\0';
    r++;
    while (s[r]!='\0' && i<99){
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
    if(c==NULL){
         printf("Erreur d'allocation mémoire : ic");
         return NULL;
    }
    c->size = SIZE;
    c->T = calloc(SIZE,sizeof(kvp*));
    if(c->T==NULL){
         printf("Erreur d'allocation mémoire : ic");
         free(c);
         return NULL;
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
    if(c==NULL){
         printf("Erreur : commitSet");
         return;
    }
    unsigned long p = hash(key)%c->size ;
    while(c->T[p] != NULL ) {
        p =(p +1)%c-> size ; //probing lineaire
    }
    kvp* kv=createKeyVal(key,value);
    if(kv==NULL){
         printf("Erreur : commitSet");
         return;
    }
    c->T[p] =kv;
    c->n++;
}
Commit* createCommit(char* hash){ //deja testé
   if(hash==NULL){
         printf("Erreur : createCommit");
         return NULL;
   }
    Commit* c =initCommit();
    commitSet(c,"tree",hash);
    return c;
}

char* commitGet(Commit* c,char* key){ //deja testé
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
    int i = 0;
    int chaine_len = 0;
    int max_len = sizeof(char) * 100 * c->n;
    char *chaine = malloc(max_len*sizeof(char));
    if(chaine != NULL){
        memset(chaine,0,max_len*sizeof(char));

    }
    while (i < c->size) {
        if (c->T[i] != NULL) {
            char *temp = kvts(c->T[i]);
            if (chaine_len + strlen(temp) + 1 < max_len) {
                strcat(chaine, temp);
                strcat(chaine, "\n");
                chaine_len += strlen(temp) + 1;
            } else {
                printf("Erreur: dépassement de tampon\n");
                break;
            }
        }
        i++;
    }
    return chaine;
}


Commit *stc(char *s){ //déjà tester 
  Commit *c = initCommit();

  char *token = strtok(s, "\n");

  while(token){
    kvp* pair = stkv(token);

    commitSet(c, pair->key, pair->value);
    freeKeyVal(pair);
    token = strtok(NULL, "\n");
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

Commit *ftc(const char *file){ //déjà tester
  FILE *f = fopen(file, "r");

  if(f == NULL){
    return NULL;
  }

  size_t size = sizeof(char) * 256;
  char *s = malloc(size);

  memset(s, 0, size);

  char buf[256];
  while(fgets(buf, 256, f)){
    if(strlen(buf) + strlen(s) >= size){
      size *= 2; s = realloc(s, size);
    }
    strcat(s, buf);
  }

  Commit *c = stc(s);
  free(s);
  return c;
}
char * blobCommit ( Commit * c ) {
    char fname[100] = " /tmp/myfileXXXXXX" ;
    int fd =mkstemp(fname);
    ctf(c,fname);
    char *hash = sha256file(fname) ;
    if(hash==NULL){
         printf("Erreur de hashage \n");
         close(fd);
         return NULL;
    }
    char *ch = hashToFile(hash) ;
      if(ch==NULL){
            printf("Erreur de hashage \n");
            close(fd);
            free(hash);
            return NULL;
      }
    strcat(ch,".c");
    cp(ch,fname);
    close(fd);
    free(ch);
    return hash ;
}

void printCommit(Commit* c) {
    if (c == NULL) {
        printf("Commit vide\n");
        return;
    }

    printf("Contenu du Commit :\n");
    for (int i = 0; i < c->size; i++) {
        if (c->T[i] != NULL) {
            printf("  %s: %s\n", c->T[i]->key, c->T[i]->value);
        }
    }
}

// Fonction pour libérer la mémoire allouée pour un Commit
void freeCommit(Commit* c) {
    if (c == NULL) {
        return;
    }

    for (int i = 0; i < c->size; i++) {
        if (c->T[i] != NULL) {
            free(c->T[i]->key);
            free(c->T[i]->value);
            free(c->T[i]);
        }
    }

    free(c->T);
    free(c);
}



void initRefs(){ //deja tester
    DIR* dir = opendir(".refs");
    if(dir == NULL){
        int ret = system("mkdir .refs");
        if(ret == -1){
            printf("Erreur lors de la création du répertoire .refs\n");
            return;
        }
        ret = system("touch .refs/master");
        if(ret == -1){
            printf("Erreur lors de la création du fichier .refs/master\n");
            return;
        }
        ret = system("touch .refs/HEAD");
        if(ret == -1){
            printf("Erreur lors de la création du fichier .refs/HEAD\n");
            return;
        }
        return;
    }
    system("touch .refs/master");
    system("touch .refs/HEAD");
    closedir(dir);
}

void createUpdateRef(char* ref_name, char* hash){ //deja testé
    if(ref_name == NULL || hash == NULL){
        return;
    }
    if(file_exists(ref_name) == 0){
        char command1[256];
        sprintf(command1,"touch .refs/%s",ref_name);
        int ret = system(command1);
        if(ret == -1){
            printf("Erreur lors de la création du fichier .refs/%s\n", ref_name);
            return;
        }
        char command2[256];
        sprintf(command2,"echo %s > .refs/%s",hash,ref_name);
        ret = system(command2);
        if(ret == -1){
            printf("Erreur lors de l'écriture dans le fichier .refs/%s\n", ref_name);
            return;
        }
    }
    char command3[256];
    sprintf(command3,"echo %s > .refs/%s",hash,ref_name);
    int ret = system(command3);
    if(ret == -1){
        printf("Erreur lors de l'écriture dans le fichier .refs/%s\n", ref_name);
    }
}
void deleteRef(char* ref_name){ //déjà testé 
    if(ref_name == NULL){
        return;
    }
    char path[100];
    sprintf(path,".refs/%s",ref_name);
    if(file_exists(path) == 0){
        printf("Le fichier .refs/%s n'existe pas, il n'y a rien à supprimer!\n", ref_name);
        return;
    }
    char command[256];
    sprintf(command,"rm .refs/%s",ref_name);
    int ret = system(command);
    if(ret == -1){
        printf("Erreur lors de la suppression du fichier .refs/%s\n", ref_name);
        return;
    }
}
char* getRef(char* ref_name){ //déjà testé
    if(ref_name == NULL){
        return NULL;
    }
    char path[100];
    sprintf(path,".refs/%s",ref_name);
    if(file_exists(path) == 0){
        printf("Le fichier .refs/%s n'existe pas!\n", ref_name);
        return NULL;
    }
    FILE* f = fopen(path,"r");
    if(f == NULL){
        printf("Erreur d'ouverture du fichier .refs/%s\n", ref_name);
        return NULL;
    }
    char ligne[256];
    fgets(ligne,256,f);
    fclose(f);
    int len = strlen(ligne);
    if (len > 0 && ligne[len-1] == '\n') {
        ligne[len-1] = '\0';
    }
    char* ref = malloc(strlen(ligne)+1);
    strcpy(ref, ligne);
    return ref;
}



void createFile(char* file) { //déjà testé
    if(file == NULL) {
        printf("Le nom de fichier est NULL!\n");
        return;
    }
    char buff[256];
    sprintf(buff,"touch %s",file);
    if(system(buff) == -1) {
        printf("Erreur lors de la création du fichier %s\n", file);
    }
}
 
void myGitAdd(char* file_or_folder) {
    WorkTree* wt = NULL;
    if(file_exists(".add") == 0) {
        createFile(".add");
        wt = initWorkTree();
    } else {
        wt = ftwt(".add");
    }
    if(file_exists(file_or_folder) == 1) {
        char* hash = sha256file(file_or_folder);
        appendWorkTree(wt, file_or_folder, hash, 0);
        wttf(wt, ".add");
        free(hash);
    } else {
        printf("Fichier/répertoire %s n'existe pas\n", file_or_folder);

    }
    free(wt);
}

void myGitCommit(char *branch_name, char *message)
{
  if(!file_exists(".refs")){
    printf("Il faut d'abord initialiser les références du projets");
    return;
  }

  char path[256] = ".refs/";
  strcat(path, branch_name);
  if(!file_exists(path)){
    printf("La branche %s n'existe pas", path);
    return;
  }

  char *last_hash = getRef(branch_name);
  char *head_hash = getRef("HEAD");

  int is_different = strcmp(last_hash, head_hash);

  free(head_hash);

  if(is_different){
    printf("HEAD doit pointer sur le dernier commit de la branche %s", branch_name);
    return;
  }

  WorkTree *wt = ftwt(".add");

  if(wt == NULL){
    printf("Il faut crée le fichier .add");
    return;
  }

  char *hash = saveWorkTree(wt, ".");
  Commit *c = createCommit(hash);
  free(hash);

  if (strlen(last_hash) > 0){
    commitSet(c, "predecessor", last_hash);
  }
  if(message != NULL){
    commitSet(c, "message", message); // commit["message"] = message;
  }
  free(last_hash);

  char *commit_hash = blobCommit(c);
  createUpdateRef(branch_name, commit_hash);
  createUpdateRef("HEAD", commit_hash);

  free(commit_hash);
  freeCommit(c);
  
}



void initBranch(){
    FILE* f = fopen(".current_branch", "w");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fputs("master", f);
    fclose(f);
}

int branchExists(char* branch){
    List* refs=listdir(".refs");
    return searchList(refs,branch) !=NULL;
}

void createBranch(char* branch){
    char* hash=getRef("HEAD");
    createUpdateRef(branch,hash);
}

char* getCurrentBranch(){
    FILE* f = fopen(".current_branch","r");
    if (f == NULL){
        printf("Erreur d'ouverture du fichier .current_branch.\n");
        return NULL;
    }
    char* buff = malloc(sizeof(char) * 100);
    if (buff == NULL){
        printf("Erreur d'allocation de mémoire.\n");
        fclose(f);
        return NULL;
    }
    fscanf(f, "%s", buff); //%99s
    fclose(f);
    return buff;
}

char* hashTopathCommit(char* hash){
    if (hash == NULL){
        printf("Erreur : pointeur NULL passé en argument.\n");
        return NULL;
    }

    char* buff = malloc(sizeof(char) * 100);
    if (buff == NULL){
        printf("Erreur d'allocation de mémoire.\n");
        return NULL;
    }

    char* hashPath = hashToPath(hash);
    if (hashPath == NULL){
        printf("Erreur lors de la conversion du hash en chemin.\n");
        free(buff); 
        return NULL;
    }

    sprintf(buff, "%s.c", hashPath);
    free(hashPath); 
    return buff;
}

void printBranch(char* branch){
    char* commit_hash=(char*)getRef(branch);
    Commit* c=ftc(hashTopathCommit(commit_hash));
    while(c!=NULL){
        if(commitGet(c,"message")!=NULL)
            printf("%s -> %s \n",commit_hash,commitGet(c,"message"));
        else
            printf("%s \n",commit_hash);
        if(commitGet(c,"predecessor")!=NULL){
            commit_hash=commitGet(c,"predecessor");
            c=ftc(hashTopathCommit(commit_hash));
        }else{
            c=NULL;
        }
    }

}
void printBranch2(char* branch){
    if (branch == NULL){
        printf("Erreur : pointeur NULL passé en argument.\n");
        return;
    }

    char* commit_hash = getRef(branch);
    if (commit_hash == NULL){
        printf("Erreur lors de la récupération du hash du commit pour la branche '%s'.\n", branch);
        return;
    }

    Commit* c = ftc(hashTopathCommit(commit_hash));
    if (c == NULL){
        printf("Erreur lors de la récupération du commit pour le hash '%s'.\n", commit_hash);
        free(commit_hash); 
        return;
    }

    while(c != NULL){
        if(commitGet(c,"message") != NULL)
            printf("%s -> %s \n",commit_hash,commitGet(c,"message"));
        else
            printf("%s \n",commit_hash);
        if(commitGet(c,"predecessor") != NULL){
            commit_hash = commitGet(c,"predecessor");
            free(c); 
            c = ftc(hashTopathCommit(commit_hash));
        }else{
            free(c); 
            c = NULL;
        }
    }

    free(commit_hash); 
}

List* branchList(char* branch){
    List* L=initList();
    char* commit_hash=getRef(branch);
    Commit* c=ftc(hashTopathCommit(commit_hash));
    while(c!=NULL){
        insertFirst(L,buildCell(commit_hash));
        if(commitGet(c,"predecessor")!=NULL){
            commit_hash=commitGet(c,"predecessor");
            c=ftc(hashTopathCommit(commit_hash));
        }else{
            c=NULL;
        }
    }
    return L;
}
List* branchList2(char* branch){
    if (branch == NULL){
        printf("Erreur : pointeur NULL passé en argument.\n");
        return NULL;
    }

    List* L = initList();
    char* commit_hash = getRef(branch);
    if (commit_hash == NULL){
        printf("Erreur lors de la récupération du hash du commit pour la branche '%s'.\n", branch);
        return NULL;
    }

    Commit* c = ftc(hashTopathCommit(commit_hash));
    if (c == NULL){
        printf("Erreur lors de la récupération du commit pour le hash '%s'.\n", commit_hash);
        free(commit_hash);
        return NULL;
    }

    while(c != NULL){
        insertFirst(L, buildCell(commit_hash));
        if(commitGet(c,"predecessor") != NULL){
            commit_hash = commitGet(c,"predecessor");
            free(c); 
            c = ftc(hashTopathCommit(commit_hash));
        }else{
            free(c); 
            c = NULL;
        }
    }

    free(commit_hash); 
    return L;
}


List* getAllCommits(){
    List* L = initList();
    List* content = listdir(".refs");
    if (content == NULL){
        printf("Erreur lors de la récupération du contenu du répertoire '.refs'.\n");
        return NULL;
    }

    for(Cell* ptr = *content; ptr != NULL; ptr = ptr->next) {
        if (ptr->data[0] == '.')
            continue;
        List* list = branchList(ptr->data);
        if (list == NULL){
            printf("Erreur lors de la récupération de la liste des commits pour la branche '%s'.\n", ptr->data);
            freeList(content);
            freeList(L);
            return NULL;
        }

        Cell* cell = *list;
        while(cell != NULL ) {
            if (searchList(L, cell->data) == NULL) {
                insertFirst(L, buildCell(cell->data));
            }  
            cell = cell->next;
        }

        freeList(list); 
    }

    freeList(content); 
    return L;
}
void printList(List* l) {
    if (l == NULL) {
        printf("Liste vide.\n");
    } else {
        Cell* current = *l; // Début de la liste
        while (current != NULL) {
            printf("%s\n", current->data); // Supposons que les données de la liste sont des chaînes de caractères
            current = current->next; // Passage au nœud suivant dans la liste
        }
    }
}
int main() {
    printf("Initialisation de la branche 'master'\n");
    initBranch();

    printf("Création d'une nouvelle branche 'develop'\n");
    createBranch("develop");

    printf("Vérification de l'existence de la branche 'develop'\n");
    if (branchExists("develop")) {
        printf("La branche 'develop' existe.\n");
    } else {
        printf("La branche 'develop' n'existe pas.\n");
    }

    printf("Vérification de l'existence de la branche 'feature'\n");
    if (branchExists("feature")) {
        printf("La branche 'feature' existe.\n");
    } else {
        printf("La branche 'feature' n'existe pas.\n");
    }

    printf("Récupération de la branche courante\n");
    char* currentBranch = getCurrentBranch();
    if (currentBranch != NULL) {
        printf("La branche courante est : %s\n", currentBranch);
        free(currentBranch);
    }

    printf("Affichage du contenu de la branche 'master'\n");
    printf("Branche 'master' :\n");
    printBranch("master");

    printf("Affichage du contenu de la branche 'develop'\n");
    printf("Branche 'develop' :\n");
    printBranch("develop");

    printf("Liste des commits de la branche 'develop'\n");
    List* branchCommits = branchList("develop");
    if (branchCommits != NULL) {
        printf("Commits de la branche 'develop':\n");
        printList(branchCommits);
        freeList(branchCommits);
    }

    printf("Liste des commits de la branche 'master'\n");
    List* masterCommits = branchList("master");
    if (masterCommits != NULL) {
        printf("Commits de la branche 'master':\n");
        printList(masterCommits);
        freeList(masterCommits);
    }

    printf("Liste de tous les commits\n");
    List* allCommits = getAllCommits();
    if (allCommits != NULL) {
        printf("Tous les commits:\n");
        printList(allCommits);
        freeList(allCommits);
    }

    return 0;
}