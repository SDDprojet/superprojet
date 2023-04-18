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
    printf("--------------------Exercice 1, 2 et 3--------------------\n");
    printf("\n");
    printf("\n");
	List* l = initList();

	Cell* c1 = buildCell("test");
	Cell* c2 = buildCell("ahh");
	Cell* c3 = buildCell("HZHE");
	
	insertFirst(l,c1);
	insertFirst(l,c2);
	insertFirst(l,c3);
    ltof(l,"test_ltof2222.txt");
    List* l3 = stol("chaine1|chaine2|chaine3|test|1");
    printf("test stol => %s\n",ltos(l3)); 
    printf("\n\n hastToPath avec fichier main.c = %s \n",hashToPath(sha256file("main.c")));
    blobFile("main.c");
    printf("\n");
    printf("\n");
    List* l2 = ftol("test_ltof.txt");
    printf("%s\n",ltos(l));
    printf("ici c'est l2 %s\n",ltos(l));
	printf("%s\n",ctos(listGet(l,1)));
    List* m1 = listdir(".");
    printf("%s\n",ltos(m1)); 
	printf("%s\n",searchList(l,"ahh"));


    printf("\n");
    List* m = listdir(".");
    printf("%s\n",ltos(m)); 
    printf("\n");
    printf("\n");
    printf("---------------exercice 4 -------------\n---------------Workfile/worktree-----------\n");
    printf("\n");
    printf("\n");
    WorkFile* wifi = createWorkFile("waifu");
    printf("%s \n ",wfts(wifi)); 
    WorkFile* waaa = stwf(wfts(wifi));
    printf("wa = %s \n ",wfts(waaa));
    WorkTree* watermelon = initWorkTree();
    appendWorkTree(watermelon,"water","12345463",1);
    appendWorkTree(watermelon,"worm","5555555",1);

    printf("la présence de fichier de worktree : %d \n",inWorkTree(watermelon,"water"));
    
    printf("watermelon =\n %s  \n",wtts(watermelon));
    printf("wttf = %d \n",wttf(watermelon,"warintaralala"));
    printf("ftwt =\n %s \n",wtts(ftwt("warintaralala")));
    printf("\n");
    printf("--------------------Exercice 5--------------------\n");
    printf("\n");
    printf("blobWorkTree(watermelon) = %s \n ",blobWorkTree(watermelon));
    printf("hash de safeWorktree %s \n",saveWorkTree(watermelon,"."));
    restoreWorkTree(watermelon,".");
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
    printf("-------------------Exercice 7-----------------\n");
    printf("\n");
    initRefs();
    createBranch("heheh");
    createUpdateRef("master","123456");
    createUpdateRef("massyl","est-ce que tu sais que je taime");
    printf("%s \n",ltos(listdir(".refs")));
    printf("%d\n",file_exists(".refs/master"));
    printf("ref de HEAD = %s \n",getRef("HEAD"));
    myGitAdd("main.c");
    myGitCommit("master","modif");
    printf("%s \n",ltos(listdir(".refs")));
    //myGitCommit("master","modif");


    printf("\n");
    printf("\n");
    printf("-------------------Exercice 8-----------------\n");
    printf("\n");
    printf("\n");

    //createBranch("hahahhaaaaaa");
    //désallouer
    freeList(l);
    freeList(l3);
    freeList(m);
    freeList(m1);
    //freeCommit()
    freeKeyVal(k);
    free(k2);
    //freeWorkTree(watermelon);



	return 0; 
}