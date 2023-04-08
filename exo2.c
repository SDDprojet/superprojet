#define SIZE 20

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

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
typedef struct cell {
	char* data ;
	struct cell* next ;
} Cell ;

typedef Cell* List ;

List* initList(){
    List* l=(List*)(malloc(sizeof(List)));
    *l=NULL;
    return l;
}

Cell* buildCell(char* ch){
    Cell* c = (Cell*)(malloc(sizeof(Cell)));
    c->data = strdup(ch);
    c->next = NULL;

    return c;
}
void freeCell(Cell* c){
    while(c != NULL){
        free(c->data);
        c = c->next;
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
    Cell * tmp = *L;
    while(tmp){
        strcat(res, ctos(tmp));
        strcat(res, "|");
        tmp = tmp->next;
    }
    return res;
}
Cell* listGet(List* l, int i){
	while(*l){
		if(i==1){
			return *l;
		}
		*l=(*l)->next;
		i--;
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
    if(dp != NULL)
    {
        while ((ep = readdir(dp)) != NULL)
        {   Cell* c1 = buildCell(ep->d_name);
            insertFirst(L,c1);
        }
    }
    return L;
}

void insertLast(List *L, Cell *C){
    List l = (*L);
    if (*L == NULL){
        *L = C;
        return;
    }
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
int file_exists(char *file){
    List* l = listdir(".");
    Cell* c = searchList(l,file);
    if(c == NULL){
        return 0;
    }
    return 1;
}
void cp(char *to, char *from){
    if(file_exists(to)==0){
        printf("le fichier %s n'existe pas\n",to );
        return ;
    }
    FILE * source = fopen(from, "r");
    FILE * dest = fopen(to, "w");
    char buffer[256];
    while(fgets(buffer,256,source)!=NULL){
        fprintf(dest,"%s",buffer);
    }
    fclose(source);
    fclose(dest);

}

char* hashToPath(char* hash){
    char* res = malloc(strlen(hash)+1);
    strcpy(res,hash);
    sprintf(res, "%c%c/%s", hash[0],hash[1],hash+2);
    return res;
}

void blobFile(char* file){
    if(file_exists(file)==0){
        printf("Erreur : le fichier %s n'existe pas\n", file);
        return ;
    }
    char* hash = sha256file(file);
    char* chemin = hashToPath(hash);
    char command1[500];
    sprintf(command1, "mkdir %c%c",chemin[0],chemin[1]);
    system(command1);
    cp(chemin,file);
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


void blobFile(char* file){
    char* hash = sha256file(file) ;
    char* ch2 = strdup(hash) ;
    ch2[2] = '\0' ;
    if (!file_exists(ch2)){
       char buff[100];
       sprintf(buff,"mkdir %s",ch2);
       system(buff);
    }
    char* ch=hashToPath(hash);
    cp(ch,file);
 }
 char* saveWorkTree(WorkTree* wt, char* path){
    int i;
    for(i = 0; i < wt->n; i++) {
        WorkFile* WF = &(wt->tab[i]);
        char* hash = NULL;
        // If WF corresponds to a file
        if(WF->mode == 0) {
            // Create an instant snapshot of the file
            hash = sha256file(WF->name);
            // Save the hash and mode in WF
            WF->hash = hash;
            WF->mode = strtol(hash, NULL, 16) & 0777;
        }
        // If WF corresponds to a directory
        else if(WF->mode == 1) {
            // Create a new WorkTree representing the contents of the directory
            WorkTree newWT;
            newWT.tab = NULL;
            newWT.n = 0;
            listdir(WF->name, &newWT);
            // Recursively call saveWorkTree on the new WorkTree
            saveWorkTree(&newWT, WF->name);
            // Save the hash and mode in WF
            WF->hash = newWT.tab[0].hash;
            WF->mode = newWT.tab[0].mode;
            // Free the memory allocated for the new WorkTree
            free(newWT.tab);
        }
    }

    // Call blobWorkTree on wt to create its instant snapshot and return its hash
    char* wtHash = blobWorkTree(wt);
    return wtHash;
}
void restoreWorkTree(WorkTree* wt, char* path) {
    List* L = ftol(path); // Charger la liste depuis le fichier

    Cell* c = *L;
    while (c != NULL) {
        char* hash = ctos(c); // Récupérer le hash depuis la cellule
        char* file_path = hashToPath(hash); // Convertir le hash en chemin de fichier
        char* source_path = malloc(strlen(wt->dir)+strlen(file_path)+2); // Allouer de la mémoire pour le chemin complet
        sprintf(source_path, "%s/%s", wt->dir, file_path); // Construire le chemin complet
        char* dest_path = malloc(strlen(wt->dir)+strlen(ctos(c))+2); // Allouer de la mémoire pour le chemin complet de destination
        sprintf(dest_path, "%s/%s", wt->dir, ctos(c)); // Construire le chemin complet de destination

        cp(dest_path, source_path); // Copier le fichier depuis le chemin source vers le chemin de destination
        free(source_path); // Libérer la mémoire allouée pour le chemin source
        free(dest_path); // Libérer la mémoire allouée pour le chemin de destination

        c = c->next; // Passer à la cellule suivante
    }

    freeCell(*L); // Libérer la mémoire allouée pour la liste
    free(L); // Libérer la mémoire allouée pour le pointeur de liste
}
 /*char* saveWorkTree(WorkTree* wt, char* path) {
    // Parcours du tableau de WorkFile de wt
    for (int i = 0; i < wt->n; i++) {
        WorkFile* wf = &(wt->tab[i]);
        // Si wf correspond à un fichier
        if (!strstr(wf->name, ".t")) {
            // Création de l'enregistrement instantané du fichier
            blobFile(wf->name, wf->hash, &(wf->mode));
            // Sauvegarde du hash et du mode dans wf
            wf->hash = strdup(wf->hash);
            wf->mode = getChmod(wf->name);
        }
        // Si wf correspond à un répertoire
        else {
            // Création d'un nouveau WorkTree pour représenter le contenu du répertoire
            WorkTree newWT;
            newWT.name = strdup(wf->name);
            newWT.n = 0;
            newWT.tab = NULL;
            // Appel récursif sur le nouveau WorkTree
            char* newHash = saveWorkTree(&newWT, path);
            // Sauvegarde du hash et du mode dans wf
            wf->hash = strdup(newHash);
            wf->mode = getChmod(wf->name);
        }
    }
    // Création de l'enregistrement instantané du WorkTree wt
    char* wtHash = (char*)malloc(10 * sizeof(char)); // Exemple de hash pour le WorkTree wt
    blobWorkTree(wt, wtHash);
    return wtHash;
}

void restoreWorkTree(WorkTree* wt, char* path) {
    int i;
    for (i = 0; i < wt->count; i++) {
        WorkFile* wf = &(wt->tab[i]);

        // Trouver l'enregistrement instantané correspondant au hash de WF
        char* hash = wf->hash;
        Blob* blob = findBlobByHash(hash);

        if (blob == NULL) {
            printf("Erreur : impossible de trouver l'enregistrement instantané pour le fichier %s\n", wf->name);
            continue;
        }

        // Si l'enregistrement ne possède pas l'extension ".t", il s'agit d'un fichier
        if (strstr(blob->name, ".t") == NULL) {
            // Créer une copie de l'enregistrement à l'endroit indiqué par la variable path
            char* newFilePath = createFilePath(path, wf->name);
            FILE* file = fopen(newFilePath, "wb");
            if (file == NULL) {
                printf("Erreur : impossible de créer le fichier %s\n", newFilePath);
                continue;
            }

            fwrite(blob->content, 1, blob->size, file);
            fclose(file);

            // Modifier les autorisations du fichier selon le mode de l'enregistrement
            chmod(newFilePath, wf->mode);
            printf("Restauré : %s\n", newFilePath);
        }
        // Si l'enregistrement possède l'extension ".t", il s'agit d'un répertoire
        else {
            // Créer le WorkTree associé
            WorkTree* newWT = parseWorkTree(blob->content, blob->size);

            // Modifier la variable path en y ajoutant ce répertoire à la fin
            char* newPath = createFilePath(path, wf->name);

            // Faire un appel récursif sur le nouveau WorkTree
            restoreWorkTree(newWT, newPath);

            free(newWT);
        }
    }
}*/
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

/*----------------------Exercice 6----------------------*/
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

kvp* createKeyVal(char* key,char* val){
    kvp* k = malloc(sizeof(kvp));
    k->key = strdup(key);
    k->value = strdup(val);
    return k ;
}

void freeKeyVal(kvp* kv){
    free(kv->key);
    free(kv->value);
    free(kv);
}

char* kvts(kvp* k){
/* permet de convertir un élément en une chaine de 
caractères de la forme "clé :valeur" */
	if(k == NULL){
		return NULL;
	}
	char *chaine = (char *)malloc(sizeof(char)*255);
	sprintf(chaine,"%s :%s",k->key,k->value);
	return chaine;
}

kvp* stkv(char* s){
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

Commit* initCommit(){
    Commit* c = malloc(sizeof(Commit));
    kvp* tab = malloc(sizeof(kvp));
    c-> n = 0;
    c->size = SIZE;
    c->T = tab;
    for(int i = c->n; i< c->size ; i++ ){
        c->T[i]->key = NULL;
        c->T[i]->Value = NULL;
    }
    return c;
}

unsigned long hash(unsigned char *str){


	unsigned int hash = 0;
	int c;

	while (c = *str++)
	    hash += c;

	return hash;
    }


void commitSet(Commit* c,char* key, char* value){    
    int p = hash(key)%c->size ;
    while(c->T[p] != NULL ) {
        p =(p +1)%c-> size ; //probing lineaire
    }
    c->T[p] =createKeyVal(key,value);
    c->n++;
}
Commit* createCommit(char* hash){
    Commit* c =initCommit();
    commitSet(c,"tree",hash);
    return c;
}

char* commitGet(Commit* c,char* key){
    int i = 0;
    while(i != c->n){
        if(strcmp(c->T[i]->key,key)==0){
            return c->T[i]->value;
        }
        i++;
    }
    return NULL;
}

char* cts(Commit* c){
    int i =0;
    char* chaine;
    while(i != c->n){
        sprintf(chaine,"%s \n",kvts(c->T[i]));
    }
    return chaine ;
}

void ctf(Commit* c, char* file){
    FILE* dest = fopen(file);
    if(dest == NULL){
        printf("Problème d'ouverture fichier \n");
        return ;
    }

    

Commit* ftc(char* file){
    Commit c = initCommit();
    return c;
}
int main(){
	List* l = initList();

	Cell* c1 = buildCell("test");
	Cell* c2 = buildCell("ahh");
	Cell* c3 = buildCell("HZHE");
	
	insertFirst(l,c1);
	insertFirst(l,c2);
	insertFirst(l,c3);
    ltof(l,"test_ltof.txt");
    List* l3 = stol("warintara|the|pretiest|my |love");
    printf("%s\n",ltos(l3)); 
    //printf("\n\n hastToPath avec fichier main.c = %s \n",hashToPath(sha256file("main.c")));
    blobFile("test_ltof.txt");
    
    WorkFile* wifi = createWorkFile("waifu");
    printf("%s \n ",wfts(wifi)); 
    WorkFile* waaa = stwf(wfts(wifi));
    printf("wa = %s \n ",wfts(waaa));
    WorkTree* watermelon = initWorkTree();
    appendWorkTree(watermelon,"water","12345463",1);
    appendWorkTree(watermelon,"worm","5555555",1);

    printf("la présence de fichier de worktree : %d \n",inWorkTree(watermelon,"water"));
    
    printf("watermelon =\n %s  \n",wtts(watermelon));
    printf("wttf = %d \n",wttf(watermelon,"warintaralala"));
    printf("ftwt =\n %s \n",wtts(ftwt("warintaralala")));
    printf("--------------------Exercice 5--------------------\n");
    printf("blobWorkTree(watermelon) = %s \n ",blobWorkTree2(watermelon));
    

    //List* l2 = ftol("test_ltof.txt");
    //printf("%s\n",ltos(l));
    //printf("ici c'est l2 %s\n",ltos(l));

	//printf("%s\n",ctos(listGet(l,1)));
    //List* m = listdir(".");
    //printf("%s\n",ltos(m)); 
	//printf("%s\n",searchList(l,"ahh"));


    printf("--------------------------------------------\n");
    List* m = listdir(".");
    printf("%s\n",ltos(m)); 
    printf("%d \n",file_exists("exo2.c"));

    printf("\n");
    printf("\n");
    printf("-------------------Exercice 6-----------------\n");
    //printf("stkv de la chaîne 'Massyl:Bengana' = %s \n",kvts(stkv("Massyl:Bengana")));
	return 0; 
}
