#include "filehash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int hashFile (char* source, char* dest) {

    char cmd[256] = "\0";
    sprintf(cmd, "sha256sum \"%s\" > \"%s\"", source, dest);
    return system(cmd);
}

char* sha256file(char* file){
    static char template[] = "/tmp/myfileXXXXXX";
    char fname[100];
    strcpy(fname,template);
    mkstemp(fname);
    char command1[200];
    sprintf(command1, "cat %s | sha256sum > %s",file,fname);
    system(command1);
    FILE* f = fopen(fname,"r");
    if (f==NULL) {
        printf("Erreur ouverture\n");
        exit(EXIT_FAILURE);
    }
    char* hash = (char*)malloc(1000*sizeof(char));
    fscanf(f,"%s",hash);
    fclose(f);
    char command2[150];
    sprintf(command2, "rm %s",fname);
    system(command2);
    return hash;
}