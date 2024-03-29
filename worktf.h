#ifndef worktf_H
#define worktf_H

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

int getChmod(const char * path);

void setMode(int mode , char* path);

WorkFile* createWorkFile(char* name);

char* wfts(WorkFile* wf);

WorkFile* stwf(char* ch);

WorkTree* stwt(char* ch);

WorkTree* initWorkTree();

int inWorkTree(WorkTree* wt, char* name);

void freeWorkTree(WorkTree* wt);

int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode);
/*qui ajoute un fichier ou r ́epertoire au WorkTree (s’il n’existe pas d éjà*/

char* wtts(WorkTree* wt);

int wttf(WorkTree* wt, char* file);

WorkTree* ftwt(char* file);

char* hashToFile (char* hash );

char* workTreeToPath(const char* hash);


char* blobWorkTree(WorkTree* wt);

void restoreWorkTree(WorkTree* wt, char* path);

char* conct(char* c1, char* c2);

int isFile(const char* name);

int isFile(const char* name);

char* saveWorkTree(WorkTree* wt, char* path);


int isWorkTree(char* hash);

void restoreWorkTree(WorkTree* wt, char* path);

#endif 