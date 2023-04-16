#include "ref.h"
#include "worktf.h"
#include "filehash.h"
#include "liste.h"
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

void initRefs(){ //deja tester
    DIR* dir = opendir(".refs");
    if(dir == NULL){
        int ret = system("mkdir .refs");
        if(ret == -1){
            printf("Erreur lors de la création du répertoire .refs\n");
            return;
        }
        ret = system("touch .refs/master");
        if(ret == -1){
            printf("Erreur lors de la création du fichier .refs/master\n");
            return;
        }
        ret = system("touch .refs/HEAD");
        if(ret == -1){
            printf("Erreur lors de la création du fichier .refs/HEAD\n");
            return;
        }
        return;
    }
    system("touch .refs/master");
    system("touch .refs/HEAD");
    closedir(dir);
}

void createUpdateRef(char* ref_name, char* hash){ //deja testé
    if(ref_name == NULL || hash == NULL){
        return;
    }
    if(file_exists(ref_name) == 0){
        char command1[256];
        sprintf(command1,"touch .refs/%s",ref_name);
        int ret = system(command1);
        if(ret == -1){
            printf("Erreur lors de la création du fichier .refs/%s\n", ref_name);
            return;
        }
        char command2[256];
        sprintf(command2,"echo %s > .refs/%s",hash,ref_name);
        ret = system(command2);
        if(ret == -1){
            printf("Erreur lors de l'écriture dans le fichier .refs/%s\n", ref_name);
            return;
        }
    }
    char command3[256];
    sprintf(command3,"echo %s > .refs/%s",hash,ref_name);
    int ret = system(command3);
    if(ret == -1){
        printf("Erreur lors de l'écriture dans le fichier .refs/%s\n", ref_name);
    }
}
void deleteRef(char* ref_name){ //déjà testé 
    if(ref_name == NULL){
        return;
    }
    char path[100];
    sprintf(path,".refs/%s",ref_name);
    if(file_exists2(path) == 0){
        printf("Le fichier .refs/%s n'existe pas, il n'y a rien à supprimer!\n", ref_name);
        return;
    }
    char command[256];
    sprintf(command,"rm .refs/%s",ref_name);
    int ret = system(command);
    if(ret == -1){
        printf("Erreur lors de la suppression du fichier .refs/%s\n", ref_name);
        return;
    }
}
char* getRef(char* ref_name){ //déjà testé
    if(ref_name == NULL){
        return NULL;
    }
    char path[100];
    sprintf(path,".refs/%s",ref_name);
    if(file_exists2(path) == 0){
        printf("Le fichier .refs/%s n'existe pas!\n", ref_name);
        return NULL;
    }
    FILE* f = fopen(path,"r");
    if(f == NULL){
        printf("Erreur d'ouverture du fichier .refs/%s\n", ref_name);
        return NULL;
    }
    char ligne[256];
    fgets(ligne,256,f);
    fclose(f);
    int len = strlen(ligne);
    if (len > 0 && ligne[len-1] == '\n') {
        ligne[len-1] = '\0';
    }
    char* ref = malloc(strlen(ligne)+1);
    strcpy(ref, ligne);
    return ref;
}

void createFile(char* file) { //déjà testé
    if(file == NULL) {
        printf("Le nom de fichier est NULL!\n");
        return;
    }
    char buff[256];
    sprintf(buff,"touch %s",file);
    if(system(buff) == -1) {
        printf("Erreur lors de la création du fichier %s\n", file);
    }
}
 
void myGitAdd(char* file_or_folder) {
    WorkTree* wt = NULL;
    if(file_exists2(".add") == 0) {
        createFile(".add");
        wt = initWorkTree();
    } else {
        wt = ftwt(".add");
    }
    if(file_exists2(file_or_folder) == 1) {
        char* hash = sha256file(file_or_folder);
        appendWorkTree(wt, file_or_folder, hash, 0);
        wttf(wt, ".add");
        free(hash);
    } else {
        printf("Fichier/répertoire %s n'existe pas\n", file_or_folder);

    }
    free(wt);
}

void myGitCommit(char* branch_name, char* message) {
    if(file_exists(".refs") == 0) {
        printf("Initialiser d'abord les références du projet\n");
        return;
    }
    // Comparer si HEAD et branch_name pointent vers la même chose
    char ch[256];
    sprintf(ch, ".refs/%s", branch_name);
    if(file_exists(ch) == 0) {
        printf("La branche n'existe pas\n");
        return;
    }
    char* bn_ref = getRef(branch_name);
    char* head_ref = getRef("HEAD");
    if(strcmp(bn_ref, head_ref) != 0) {
        printf("HEAD doit pointer sur le dernier commit de la branche\n");
        return;
    }
    // Charger worktree puis le supprimer
    WorkTree* wt = ftwt(".add");
    system("rm .add");
    // Enregistrer instantané de ce worktree et récupérer son hash
    char* hashwt = saveWorkTree(wt, ".");
    // Créer commit c
    Commit* c = createCommit(hashwt);
    // Lire le fichier branch_name
    char* predecessor = getRef(branch_name);

    if(strlen(predecessor) != 0) {
        commitSet(c, "predecessor", bn_ref);
    }
    if(message != NULL) {
        commitSet(c, "message", message);
    }
    char* commit_hash = blobCommit(c);
    createUpdateRef(branch_name, commit_hash);
    createUpdateRef("HEAD", commit_hash);
}
/*
int main() {
    // Exemple d'utilisation des fonctions

    // Utilisation de la fonction getRef
    char* ref = getRef("my_branch");
    if(ref == NULL) {
        printf("Erreur lors de l'appel à getRef\n");
    } else {
        printf("Référence : %s\n", ref);
    }

    // Utilisation de la fonction createFile
    createFile("myfile.txt");
    printf("Fichier créé : myfile.txt\n");

    // Utilisation de la fonction myGitCommit
    myGitCommit("my_branch", "Mon message de commit");
    printf("Commit effectué avec succès\n");

    return 0;
}

*/
    
