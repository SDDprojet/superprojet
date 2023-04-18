#include "checkout.h"
#include "worktf.h"
#include "filehash.h"
#include "liste.h"
#include "branch.h"
#include "ref.h"
#include "commit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#define SIZE 20


WorkTree *mergeWorkTrees(WorkTree *wt1, WorkTree *wt2, List **conflicts){
    WorkTree *wt = initWorkTree();

  // verifier si il y a le conflit 
    if(wt1){
    for(int i = 0; i < wt1->n; i++)
        for(int j = 0; j < wt2->n; j++)
            if( (!strcmp(wt1->tab[i].name, wt2->tab[j].name)) && ( (strcmp(wt1->tab[i].hash, wt2->tab[j].hash)) || (wt1->tab[i].mode != wt2->tab[j].mode) ) )
                insertFirst(*conflicts, buildCell(wt1->tab[i].name));
    }

  // On rajoute les éléments de wt1
    if(wt1){
        for(int i = 0; i < wt1->n; i++){
            Cell* c1 = searchList(*conflicts, wt1->tab[i].name);
            if(!c1){
                appendWorkTree(wt, wt1->tab[i].name, wt1->tab[i].hash, wt1->tab[i].mode);
            }
        }
    }
    // On rajoute les éléments de wt2
    if(wt2){
        for(int i = 0; i < wt2->n; i++){
            Cell* c2 = searchList(*conflicts, wt2->tab[i].name);
            if(!c2){
                appendWorkTree(wt, wt2->tab[i].name, wt2->tab[i].hash, wt2->tab[i].mode);
            }
        }
    }

    return wt;
}

List* merge(char* remote_branch, char* message){
    char *current_branch_nom = getCurrentBranch();
    char *current_branch = getRef(current_branch_nom);

    char *path_commit = commitPath(current_branch);
    if(path_commit == NULL){
        printf("Impossible de trouver le chemin du fichier correspondant à la branche : ");
    }
    
    char *remote_commit_path = commitPath(getRef(remote_branch));

    Commit *commit_actuel = ftc(path_commit);

    Commit *remote_commit = ftc(remote_commit_path);
    char *wtActuel_hash = commitGet(commit_actuel, "tree");
    char *remote_wt_hash = commitGet(remote_commit, "tree");
    WorkTree *wtActuel = ftwt(workTreePath(wtActuel_hash));

    WorkTree *remote_wt = ftwt(workTreePath(remote_wt_hash));

    List *conflicts = initList();

    WorkTree* merged_wt = mergeWorkTrees(wtActuel, remote_wt, &conflicts);

    if(sizeList(conflicts) > 0){
        printf("La fusion de la branche  et rencontre des conflits");
        return conflicts;
    }

    char *hash = saveWorkTree(merged_wt, ".");  

    Commit *merged_commit = createCommit(hash);
    
    commitSet(merged_commit, "predecessor", wtActuel_hash);
    commitSet(merged_commit, "merged_predecessor", remote_wt_hash);

    if(message != NULL){
        commitSet(merged_commit, "message", remote_wt_hash);
    }
    
    const char* commit_hash = blobCommit(merged_commit);

    createUpdateRef(current_branch_nom, (char*)commit_hash);
    createUpdateRef("HEAD", (char*)commit_hash);

    deleteRef(remote_branch);

    restoreCommit((char*)commit_hash);

    free(commit_actuel);
    free(remote_commit);
    free(remote_wt);
    free(wtActuel);
    free(merged_wt);
    free(wtActuel_hash);
    free(remote_wt_hash);
    free(hash);
    freeList(conflicts);
    free(current_branch);
    free(current_branch_nom);
    free(path_commit);
    free(remote_commit_path);
    free(merged_commit);
    free((char*)commit_hash);

    return NULL;
}

void createDeletionCommit(char *branch, List *conflicts, char* message){
    char *branchCour = getCurrentBranch();
    myGitCheckoutBranch((char*)branch);

    char *branch_hash = getRef((char*)branch);
    if(branch_hash == NULL){
        printf("Récupérer la référence semble impossible...");
        free(branchCour);
        return;
    }

    char *commit_path = commitPath(branch_hash);
    if(commit_path == NULL){
        printf("Transformer le hash en chemin a renvoyé null");
        free(branch_hash);  
        free(branchCour);
        return;
    }
    free(branch_hash);


    Commit *commit = ftc(commit_path);
    if(commit == NULL){
        printf("La conversion du fichier en commit a renvoyé null");
        free(commit_path);
        free(branchCour);
        return;
    }
    free(commit_path);

    char *wt_hash = commitGet(commit, "tree");
    free(commit);
    if(wt_hash == NULL){
        printf("Commit[\"tree\"] = null");
        free(branchCour);
        return;
    }

    char *wt_path = workTreePath(wt_hash);
    if(wt_path == NULL){
        printf("La conversion en chemin a échoué..");
        free(wt_hash);
        free(branchCour);
        return;
    }
    free(wt_hash);
    
    WorkTree *wt = ftwt(wt_path);
    if(wt == NULL){
        printf("ftwt a renvoyé null ");
        free(branchCour);
        free(wt_path);
        return;
    }
    free(wt_path);
        

    remove(".add");

    createFile(".add");

    for(int i = 0; i < wt->n; i++)
        if(searchList(conflicts, wt->tab[i].name))
        myGitAdd(wt->tab[i].name);

    freeWorkTree(wt);

    myGitCommit((char*)branch, (char*)message);

    myGitCheckoutBranch(branchCour);

    free(branchCour);
}