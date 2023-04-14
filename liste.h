#ifndef LISTE_H

#define LISTE_H

typedef struct cell{
char* data ;
struct cell* next ;
}Cell ;

typedef Cell* List ;


List* initList();
// initialise une liste vide

Cell* buildCell(char* ch) ;
// alloue et de retourne une cellule de la liste

void freeCell(Cell* c);
//déallouer une cellule 

void insertFirst(List* L, Cell* C);
// ajoute un element en tete d’une liste


void insertLast(List *L, Cell *C);
// ajoute un element en fin d'une liste

char* ctos(Cell* c);
// retourne la chaine de caracteres qu’elle represente

char* ltos(List* L);
// transforme une liste en une chaine de caracteres

Cell* listGet(List* L, int i);
// renvoie le i eme element d’une liste

Cell* searchList(List* L, char* str);
// recherche un element dans une liste a partir de son contenu

List* listdir(char* root_dir);
//donner une liste des fichiers existés dans ce chemin 

int file_exists(char *file);
//indique si file existe retourne 1, 0 sinon 

List* stol(char* s);
// transforme une chaine de caracteres representant une liste en une liste chainee

void ltof(List* L, char* path);
// transforme une chaine de caracteres representant une liste en une liste chainee

List* ftol(char* path);
// transforme une chaine de caracteres representant une liste en une liste chainee

void cp(char *to, char *from);

char* hashToPath(char* hash);

void blobFile(char* file);

void blobFile2(char* file);

int size_list(List l);
// envoyer la taille de la liste 

#endif