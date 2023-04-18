#ifndef checkout_H
#define checkout_H

#include "liste.h"

void myGitCheckoutBranch(char* branch);

void restoreCommit(char* hash_commit);

List* filterList(List* L, char* pattern);

void myGitCheckoutCommit(char* pattern);

char* workTreePath(char* hash);

int sizeList(List *L);
#endif