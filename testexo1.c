
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

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
int file_exists(char *file){
    List* l = listdir(".");
    Cell* c = searchList(l,file);
    freeList(l);
    if(c == NULL){
        return 0;
    }
    return 1;
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


int main(){

    // Initialisation de la liste
    List* myList = initList();

    // Insertion de cellules en début de liste
    Cell* cell1 = buildCell("Hello");
    Cell* cell2 = buildCell("World");
    insertFirst(myList, cell1);
    insertFirst(myList, cell2);

    // Recherche d'une cellule dans la liste
    Cell* cell3 = searchList(myList, "World");
    if (cell3 != NULL) {
        printf("Cellule trouvée : %s\n", cell3->data);
    } else {
        printf("Cellule non trouvée\n");
    }

    // Insertion de cellules en fin de liste
    Cell* cell4 = buildCell("Test");
    Cell* cell5 = buildCell("Liste");
    insertLast(myList, cell4);
    insertLast(myList, cell5);

    // Conversion de liste en chaîne de caractères
    char* listToString = ltos(myList);
    printf("Liste en chaîne de caractères : %s\n", listToString);

    // Conversion de chaîne de caractères en liste
    List* stringToList = stol(listToString);
    printf("Chaîne de caractères convertie en liste : ");


    // Sauvegarde de la liste dans un fichier
    ltof(myList, "liste.txt");
    printf("Liste sauvegardée dans le fichier liste.txt\n");

    // Chargement de la liste depuis un fichier
    List* loadedList = ftol("liste.txt");
    printf("Liste chargée depuis le fichier liste.txt : ");

    // Libération de la mémoire allouée
    freeList(myList);
    freeList(stringToList);
    freeList(loadedList);

    return 0;
}