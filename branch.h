#ifndef BRANCH_H
#define BRANCH_H

typedef struct cell{
char* data ;
struct cell* next ;
}Cell ;

typedef Cell* List ;

void initBranch();

int branchExists(char* branch);

void createBranch(char* branch);

char* getCurrentBranch();

char* hashTopathCommit(char* hash);

void printBranch(char* branch);

void printBranch2(char* branch);

List* branchList(char* branch);

List* getAllCommits();


#endif 