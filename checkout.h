#ifndef checkout_H
#define chcekut_H

void myGitCheckoutBranch(char* branch);

void restoreCommit(char* hash_commit);

int commence_par(char* c1, char* c2);

List* filterList(List* L, char* pattern);

void myGitCheckoutCommit(char* pattern);


#endif