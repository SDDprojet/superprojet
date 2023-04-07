#ifdef worktf_H
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

WorkTree* initWorkTree();

int inWorkTree(WorkTree* wt, char* name);

int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode);

char* wtts(WorkTree* wt);

int wttf(WorkTree* wt, char* file);

WorkTree* ftwt(char* file);

char* hashToFile (char* hash );


char* blobWorkTree2(WorkTree* wt);


#endif 