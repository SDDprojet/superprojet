#ifndef ref_h
#define ref_H

void initRefs();

void createUpdateRef(char* ref_name, char* hash);

void deleteRef(char* ref_name);

char* getRef(char* ref_name) ;

void myGitAdd(char* file_or_folder);

void myGitCommit(char* branch_name, char* message);


#endif