#ifndef ref_H
#define ref_H

void initRefs();

void createUpdateRef(char* ref_name, char* hash);

void freeCommit(Commit* c);

void deleteRef(char* ref_name);

char* getRef(char* ref_name) ;

void createFile(char* file);
void myGitAdd(char* file_or_folder);

void myGitCommit(char* branch_name, char* message);


#endif