#include "worktf.h"
#include "ref.h"
#include "checkout.h"
#include "merge.h"
#include "commit.h"
#include "filehash.h"
#include "liste.h"
#include "branch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

void create_tmp_file(){
    mkdir(".", 0700);
}

int main(int argc, char* argv[]){

    if(argc == 1){
        printf("Utilisation : \n");
        printf("%s \033[31minit\033[0m : Initialiser le repertoire \n", argv[0]);
        printf("%s \033[31mlist-refs \033[0m: Afficher toutes les references existantes.\n", argv[0]);
        printf("%s \033[31mcreate-ref <name> <hash> \033[0m: Creer la reference <name> qui pointe vers le commit correspondant au hash donne.\n", argv[0]);
        printf("%s \033[31mdelete-ref <name> \033[0m: Supprime la reference name.\n", argv[0]);
        printf("%s \033[31madd <elem> [<elem2> <elem3> ...] \033[0m: Ajoute un ou plusieurs fichiers/repertoires a la zone de preparation (pour faire partie du prochain commit).\n", argv[0]);
        printf("%s \033[31mstatus \033[0m: Affiche le contenu de la zone de preparation.\n", argv[0]);
        printf("%s \033[31mclear-add \033[0m: Vide la zone de preparation.\n", argv[0]);
        printf("%s \033[31mcommit <branch-name> [-m <message>] \033[0m: Effectue un commit sur une branche, avec ou sans message descriptif.\n", argv[0]);
        printf("%s \033[31mget-current-branch \033[0m: Affiche le nom de la branche courrante\n", argv[0]);
        printf("%s \033[31mbranch <branch-name>\033[0m: Crée la branche de nom <branch-name>\n", argv[0]);
        printf("%s \033[31mbranch-print <branch-name>\033[0m: Affiche le hash de tous les commits de la branche\n", argv[0]);
        printf("%s \033[31mcheckout-branch <branch-name>\033[0m: Réalise un déplacement de branche\n", argv[0]);
        printf("%s \033[31mcheckout-commit <pattern>\033[0m: Réalise un déplacement de commit qui commence par <pattern>\n", argv[0]);
        printf("%s \033[31mmerge\033[0m <branch> [message]: Fusionne la branche courrante et <branch>\n", argv[0]);
        return 0 ; 
    }



    if(strcmp(argv[1], "init")==0){//ok
        initRefs();
        initBranch();
        return 0;
    }

    if(strcmp(argv[1],"refs-list")==0){//ok
        printf("REFS : \n");
        if(file_exists(".refs")){
            List* L = listdir(".refs");
            for(Cell* ptr=*L; ptr!=NULL; ptr=ptr->next){
                if(ptr->data[0]=='.'){
                    continue;
                }
                char* content = getRef(ptr->data);
                printf("- %s \t %s \n", ptr->data, content);
                free(content);
            }
            freeList(L);
        }
        return 0;
    }

    if(strcmp(argv[1], "create-ref")==0){//ok
        if(argv[3]==NULL){
            createUpdateRef(argv[2], "");
            return 0;
        }
        createUpdateRef(argv[2], argv[3]);
        return 0;
    }

    if(strcmp(argv[1], "delete-ref")==0){//ok
        deleteRef(argv[2]);
        return 0;
    }

    if(strcmp(argv[1],"add")==0){//ok
        for(int i=2; i<argc; i++){
            myGitAdd(argv[i]);
        }
        return 0;
    }

    if(strcmp(argv[1], "clear-add")==0){//ok
        system("rm .add");
        return 0;
    }

    if(strcmp(argv[1], "status")==0){//ok
        printf("Zone de preparation : \n");
        if(file_exists(".add")){
            WorkTree* wt = ftwt (".add");
            char* tmp = wtts(wt);
            printf("%s \n", tmp);
            free(tmp);
            freeWorkTree(wt);
        }
        return 0;
    }

    if(strcmp(argv[1], "commit")==0){//okk
        if(strcmp(argv[3], "-m")==0){
            myGitCommit(argv[2], argv[4]);
            system("rm .add");
        }
        else{
            myGitCommit(argv[2], NULL);
        }
        return 0;
    }

     if(strcmp(argv[1], "get-current-branch")==0){//ok
        char * current_branch = getCurrentBranch();
        printf("La branche courrante est %s\n", current_branch);
        free(current_branch);
        return 0;
    }

    if(strcmp(argv[1], "branch")==0){//ok
        if(branchExists(argv[2])){
            printf("La branche %s existe déjà...\n", argv[2]);
        }
        else {
            createBranch(argv[2]);
            printf("La branche %s a été crée...\n", argv[2]);
        }
        return 0;
    }

    if(strcmp(argv[1], "branch-print")==0){//ok sauf master
        if(!branchExists(argv[2])){
            printf("La branche %s n'existe pas...\n", argv[2]);
        }
        else {
            printBranch(argv[2]);
        }
        return 0;
    }

    if(strcmp(argv[1], "checkout-branch")==0){//nok
        if(!branchExists(argv[2])){
            printf("La branche %s n'existe pas...\n", argv[2]);
        }
        else {
            myGitCheckoutBranch(argv[2]);
            printf("Réstauration de la branche %s\n", argv[2]);
        }
        return 0;
    }

    if(strcmp(argv[1], "checkout-commit")==0){//nok
        myGitCheckoutCommit(argv[2]);
        printf("Déplacement de commit vers %s\n", argv[2]);
        return 0;      
    }

    if(strcmp(argv[1], "merge")==0){
        if(branchExists(argv[2])==0){
            printf("La branche %s n'existe pas.\n", argv[2]); 
        }
        else {
            char *current_branch = getCurrentBranch();

            size_t size = sizeof(char) * 100;
            char *msg = malloc(size);
            memset(msg, 0, size);

            char delim[2] = "\0";

            for(int i = 2; ++i < argc;*delim = ' '){
                if(size >= strlen(msg) + strlen(argv[i])){
                    size <<= 2; msg = realloc(msg, size);
                }
                strcat(msg, argv[i]);
                strcat(msg, delim);
            }

            List *conflitcs = merge(argv[2], strlen(msg) ? msg : NULL);

            if(conflitcs == NULL){
                printf("Fusion de la branche %s et %s accompli!\n", current_branch, argv[2]);
            }
            else {
                printf("Impossible de fusionner, il y a conflit :\n");
                printf("1. Garder les fichier de la branche courrante (%s)\n", current_branch);
                printf("2. Garder les fichier de l'autre branche (%s)\n", argv[2]);
                printf("3. Choisir manuellement les fichiers à garder (%d fichiers)\n", sizeList(conflitcs));
                printf("4. Annuler\n");
                printf("Votre choix : ");

                int choice = -1;
                do {
                    scanf("%d", &choice);
                    printf("RESET\n");
                    if(choice < 1 || choice > 4){
                        printf("Veuillez entrez un nombre entre 1 et 4 : ");
                        fflush(stdin);
                    }
                }while(choice < 1 || choice > 4);
                
                List *new_conflitcts = NULL;

                switch (choice)
                {
                
                case 1:
                    createDeletionCommit(argv[2], conflitcs, msg);
                    new_conflitcts = merge(current_branch, argv[2]);
                    printf("Vous avez choisis de garder les fichier de la branche courrante (%s). ", current_branch);

                    if(new_conflitcts == NULL){
                        printf("Les branches ont été correctement fusionné!\n");
                    }
                    else {
                        printf("La branche %s et %s connaisse encore des conflits après la supression de celle ci\n", current_branch, argv[2]);
                    }

                    break;

                case 2:
                    createDeletionCommit(current_branch, conflitcs, msg);
                    new_conflitcts = merge(current_branch, argv[2]);
                    printf("Vous avez choisis de garder les fichier l'autre branche (%s). ", argv[2]);

                    if(new_conflitcts == NULL){
                        printf("Fin des conflits!\nLes branches ont été correctement fusionné!\n");
                    }
                    else {
                        printf("La branche %s et %s connaisse encore des conflits après la supression de celle ci\n", current_branch, argv[2]);
                    }

                    break;

                case 3:
                {
                    List* remote_suppr = initList();
                    List* current_suppr = initList();

                    for(Cell *file = *conflitcs; file != NULL; file = file->next){
                        choice = -1;
                        printf("Pour le fichier %s, de quel branche garde-t-on le fichier ?\n1. %s\n2. %s\nVotre choix : ", file->data, current_branch, argv[2]);                        
                        do {
                            scanf("%d", &choice);
                            if(choice < 1 || choice > 2){
                                printf("Choix invalide, votre choix : ");
                                fflush(stdin);
                            }
                        }while(choice < 1 || choice > 2);

                        insertFirst((choice == 1 ? remote_suppr : current_suppr), buildCell(file->data));
                    }

                    createDeletionCommit(current_branch, current_suppr, msg);
                    createDeletionCommit(argv[2], remote_suppr, msg);
                    List *new_conflitcts = merge(current_branch, msg);

                    if(new_conflitcts == NULL){
                        printf("Fin des conflits!\nLes branches ont été correctement fusionné!\n");
                    }
                    else {
                        printf("La branche %s et %s connaisse encore des conflits après la supression de celle ci", current_branch, argv[2]);
                    }

                    break;
                }

                case 4:
                    printf("Fusion annullé...\n");
                    break;
                
                default:
                    printf("Choix inconnue, annulation...\n");
                    break;
                }
            }
        }
    return 0;
    }
    
    return 0;
}