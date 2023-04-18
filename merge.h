#ifndef merge_H
#define merge_H

#include "liste.h"
#include "worktf.h"

WorkTree *mergeWorkTrees(WorkTree *wt1, WorkTree *wt2, List **conflicts);

List* merge(char* remote_branch, char* message);

void createDeletionCommit(char *branch, List *conflicts, char* message);

#endif