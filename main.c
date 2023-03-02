# include <stdio.h>
# include <stdlib.h>
#include <unistd.h>
#include <string.h>
int hashFile(char* source, char* dest){
    char buffer[256];
    sprintf(buffer,"cat %s | sha256sum >>%s",source,dest);
    system(buffer) ;
    return 0;
}
char* sha256file(char* file){
    static char template[] ="/tmp/myfileXXXXXX" ;
    char fname[1000];
    char line[256];
    strcpy(fname,template);
    int fd=mkstemp(fname) ;
    hashFile(file,fname);
    char* buffer;
    buffer=malloc(sizeof(char)*256);
    FILE* f = fopen(fname,"r");
    fgets(line,256,f); 
    sscanf(line,"%s",buffer);
    fclose(f);
    printf("%s",buffer);

    return buffer;
    

}
int main(){
    static char template [] = " /tmp/myfileXXXXXX" ;
    char fname [1000];  
    strcpy(fname,template);
    int fd = mkstemp ( fname ) ;
    int d;
    d=hashFile("main.c","file.tmp");
    char* sortie = sha256file("main.c");
    printf("%s",sortie);
}
/* 1-----------------------------------------------------------
    system("sha256sum main.c > file.tmp") ;
    system("cat file.tmp");
    */
    