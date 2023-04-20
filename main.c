#include "worktf.h"
#include "commit.h"
#include "filehash.h"
#include "liste.h"
#include "branch.h"
#include "ref.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>


int main(){
   
    printf("\n");
    printf("\n");
    printf("===================[Exercice 1, 2 et 3]===================\n");
    printf("\n");
    printf("\n");

    //ajouter et insérer les élément dans la liste
	List* l = initList();

	Cell* c1 = buildCell("test");
	Cell* c2 = buildCell("ahh");
	Cell* c3 = buildCell("HZHE");
	
	insertFirst(l,c1);
	insertFirst(l,c2);
	insertFirst(l,c3);

    //écriture un fichier avec la liste 
    ltof(l,"test_ltof.txt");
    //lecture d'un fichier 
    List* l2 = ftol("test_ltof2.txt");
    printf("apres avoir transformé test_ltof222 la liste l2  : %s\n",ltos(l2));
    
    List* l3 = stol("chaine1|chaine2|chaine3|test|1");
    printf("test stol de la chaîne 'chaine1|chaine2|chaine3|test|1' => %s\n",ltos(l3)); 

    printf("\n\n hastToPath avec fichier main.c = %s \n",hashToPath(sha256file("main.c")));
    
    blobFile("main.c");
    printf("\n");
    printf("\n");

    printf("liste to string de l1 = %s\n",ltos(l));
	printf("listGet(l,1) renvoie la valeur = %s\n",ctos(listGet(l,1)));
    List* m1 = listdir(".");
    printf("list directory de répertoire courant : %s\n",ltos(m1)); 

    char* f_path = filePath("main.c");
    printf("le path du fichier main.c = %s",f_path);
    printf("hashToPath de liste.o = %s\n",hashToPath("liste.o"));

    printf("la taille de la liste l = %d \n",size_list(l));
    printf("\n"); 
    printf("\n");
    printf("\n");
    printf("===================  exercice 4  ===================\n---------------Workfile/worktree---------------\n");
    printf("\n");
    printf("\n");
    WorkFile* wifi = createWorkFile("woody");
    printf("le nouveau WorkFile a été crée : %s \n\n ",wfts(wifi)); 
    WorkFile* waaa = stwf(wfts(wifi));
    printf("test de wfts et stwf = %s \n ",wfts(waaa));
    WorkTree* watermelon = initWorkTree();
    printf("le nouveau workTree 'watermelon 'a été crée\n");
    appendWorkTree(watermelon,"water","12345463",1);
    appendWorkTree(watermelon,"worm","5555555",1);

    printf("\n\nl'index de 'water' dans le worktree watermelon: %d \n",inWorkTree(watermelon,"water"));
    printf("l'index de 'worm' dans le worktree watermelon: %d \n",inWorkTree(watermelon,"worm"));
    printf("l'index de 'windy' dans le worktree watermelon: %d \n\n",inWorkTree(watermelon,"windy"));

    printf("le contenu de watermelon =\n %s  \n",wtts(watermelon));
    printf("essaie de stwt \n: %s\n",wtts(stwt(wtts(watermelon))));
    wttf(watermelon,"watermelon.txt");
    printf("\nftwt =\n %s \n",wtts(ftwt("watermelon.txt")));

    for(int i =0; i<watermelon->n;i++){
        printf("\nworkTree to path de watermelon : %s\n",workTreeToPath(watermelon->tab[i].hash));
        printf("hashtoFile :%s \n",hashToFile(watermelon->tab[i].hash));
    }
    printf("\n");
    printf("--------------------Exercice 5--------------------\n");
    printf("\n");
    setMode(777,"main.c");
    printf("getChmod =  %d \n",getChmod("main.c"));
    printf("blobWorkTree(watermelon) = %s \n ",blobWorkTree(watermelon));
    printf("type de main.c  = %d\n",isFile("main.c"));
    printf("hash de safeWorktree %s \n",saveWorkTree(watermelon,"."));
    //restoreWorkTree(watermelon,"."); //abandon (core dumped)
    printf("\n");
    printf("\n");
    printf("-------------------Exercice 6-----------------\n");
    printf("\n");
    kvp* k = stkv("Massyl:Bengana");
    char* k2 = kvts(k);
    printf("stkv de la chaîne 'Massyl:Bengana' = %s \n",k2);
    Commit* c=createCommit("123456");
    commitSet(c,"warintara","dreamu");
    commitSet(c,"mmomoo","mmmmm");
    
    
    printf("commit get de la clé warintara return the value = %s \n\n",commitGet(c,"warintara"));
    printf("cts de commit c : \n %s \n",cts(c));

    ctf(c,"test_ctf2");
    Commit* c_1=createCommit("2222");
    commitSet(c_1,"warintara","dreamu");
    commitSet(c_1,"mmomoo","mmmmm");

    char* test2 =cts(ftc("test_ctf2"));
    printf("ftc de test_ctf : \n %s",test2); 

    char* ss = stc(test2);
    printf("[stc] =\n %s\n",cts(ss));
    
    printf("hash de blobCommit de commit c_1 vaut = %s \n",blobCommit(c));
    printf("\n");
    printf("\n");

   

	return 0; 
}
