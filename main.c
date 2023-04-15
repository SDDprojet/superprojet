#include "worktf.h"
#include "commit.h"
#include "filehash.h"
#include "liste.h"
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
	List* l = initList();

	Cell* c1 = buildCell("test");
	Cell* c2 = buildCell("ahh");
	Cell* c3 = buildCell("HZHE");
	
	insertFirst(l,c1);
	insertFirst(l,c2);
	insertFirst(l,c3);
    ltof(l,"test_ltof2222.txt");
    List* l3 = stol("warintara|the|pretiest|my |love");
    printf("%s\n",ltos(l3)); 
    printf("\n\n hastToPath avec fichier main.c = %s \n",hashToPath(sha256file("main.c")));
    blobFile("main.c");
    
    printf("---------------Workfile-----------\n");
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
    printf("--------------------Exercice 5--------------------\n");
    printf("blobWorkTree(watermelon) = %s \n ",blobWorkTree2(watermelon));
    

    List* l2 = ftol("test_ltof.txt");
    printf("%s\n",ltos(l));
    printf("ici c'est l2 %s\n",ltos(l));
	printf("%s\n",ctos(listGet(l,1)));
    List* m1 = listdir(".");
    printf("%s\n",ltos(m1)); 
	printf("%s\n",searchList(l,"ahh"));


    printf("--------------------------------------------\n");
    List* m = listdir(".");
    printf("%s\n",ltos(m)); 
    //printf("%d \n",file_exists("exo2.c"));

    printf("\n");
    printf("\n");
    printf("-------------------Exercice 6-----------------\n");
    kvp* k = stkv("Massyl:Bengana");
    char* k2 = kvts(k);
    printf("stkv de la chaîne 'Massyl:Bengana' = %s \n",k2);
    Commit* c=createCommit("123456");
    commitSet(c,"warintara","dreamu");
    commitSet(c,"mmomoo","mmmmm");
    
    
    printf("commit get de la clé warintara return the value = %s \n",commitGet(c,"warintara"));
    printf("cts de commit c = %s \n",cts(c));

    ctf(c,"test_ctf2");

    printf("ftc de test_ctf = %s",cts(ftc("test_ctf2"))); 

    //char* ss = cts(c);
    //printf("stc = %s\n",stc(ss));
    //stc(ss)

    printf("-------------------Exercice 7-----------------\n");
    initRefs();
    createUpdateRef("master","123456");
    createUpdateRef("HEAD","123456");
    createUpdateRef("massyl","est-ce que tu sais que je taime");
    printf("%s \n",ltos(listdir(".refs")));
    printf("%d\n",file_exists2(".refs/master"));
    deleteRef("master");
    printf("ref de HEAD = %s \n",getRef("HEAD"));
    createFile("boobies");

	return 0; 
}
