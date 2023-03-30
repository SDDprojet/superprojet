#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>


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
typedef struct cell {
	char* data ;
	struct cell* next ;
} Cell ;

typedef Cell* List ;

List* initList(){
    List* l=(List*)(malloc(sizeof(List)));
    *l=NULL;
    return l;
}

Cell* buildCell(char* ch){
    Cell* c = (Cell*)(malloc(sizeof(Cell)));
    c->data = strdup(ch);
    c->next = NULL;

    return c;
}
void freeCell(Cell* c){
    while(c != NULL){
        free(c->data);
        c = c->next;
    }
}
void insertFirst(List *L,Cell* C){
    C->next=*L;
    *L=C;
    
}

char* ctos(Cell* c){
    return c->data;
}


char* ltos(List* L){
    char * res = malloc(1000);
    Cell * tmp = *L;
    while(tmp){
        strcat(res, ctos(tmp));
        strcat(res, "|");
        tmp = tmp->next;
    }
    return res;
}
Cell* listGet(List* l, int i){
	while(*l){
		if(i==1){
			return *l;
		}
		*l=(*l)->next;
		i--;
	}
	return NULL;
}


Cell* searchList(List* L, char* str){
    Cell * tete = *L;
    while(tete){
        if(strcmp(tete->data,str)==0){
            return tete;
        }
        tete = tete->next;
    }
    return NULL;
}


List* listdir(char* root_dir){
    DIR* dp = opendir(root_dir) ;
    List* L = initList();
    struct dirent *ep ;
    if(dp != NULL)
    {
        while ((ep = readdir(dp)) != NULL)
        {   Cell* c1 = buildCell(ep->d_name);
            insertFirst(L,c1);
        }
    }
    return L;
}

void insertLast(List *L, Cell *C){
    List l = (*L);
    if (*L == NULL){
        *L = C;
        return;
    }
    while(l->next){
        l=l->next;
    } 
    C->next = l->next;
    l->next = C;
}
List* stol(char* s){
    List* L = initList();
    if (s == NULL){
        return L;
    }
    char data[100];
    int r = 0;
    int i = 0;
    while (s[r]!='\0'){
        printf("%c\n",data[i]);
        if (s[r] == '|'){
            data[i] = '\0'; 
            insertLast(L, buildCell(data));
            i = 0;
        }
        else {
            data[i] = s[r];
            i++;
        }
        r++;
    }
    data[i] = '\0'; 
    insertLast(L, buildCell(data));
    return L;
}

void ltof(List* L, char* path){
    FILE * f = fopen(path, "w");
    char * chaine = ltos(L);
    fprintf(f, "%s\n",chaine);
    fclose(f);
}

List* ftol(char* path){
    FILE * f = fopen(path, "r");
    char buffer[256];
    fgets(buffer,256, f);
    List * L = stol(buffer);
    return L;   
}
int file_exists(char *file){
    List* l = listdir(".");
    Cell* c = searchList(l,file);
    if(c == NULL){
        return 0;
    }
    return 1;
}
void cp(char *to, char *from){
    if(file_exists(to)==0){
        printf("le fichier %s n'existe pas\n",to );
        return ;
    }
    FILE * source = fopen(from, "r");
    FILE * dest = fopen(to, "w");
    char buffer[256];
    while(fgets(buffer,256,source)!=NULL){
        fprintf(dest,"%s",buffer);
    }
    fclose(source);
    fclose(dest);

}

char* hashToPath(char* hash){
    char* res = malloc(strlen(hash)+1);
    strcpy(res,hash);
    sprintf(res, "%c%c/%s", hash[0],hash[1],hash+2);
    return res;
}

void blobFile(char* file){
    if(file_exists(file)==0){
        printf("Erreur : le fichier %s n'existe pas\n", file);
        return ;
    }
    char* hash = sha256file(file);
    char* chemin = hashToPath(hash);
    char command1[500];
    sprintf(command1, "mkdir %c%c",chemin[0],chemin[1]);
    system(command1);
    cp(chemin,file);
}




int main(){
	List* l = initList();

	Cell* c1 = buildCell("test");
	Cell* c2 = buildCell("ahh");
	Cell* c3 = buildCell("HZHE");
	
	insertFirst(l,c1);
	insertFirst(l,c2);
	insertFirst(l,c3);
    ltof(l,"test_ltof.txt");
    List* l3 = stol("1111|112|1|11|2");
    printf("%s\n",ltos(l3)); 
    //printf("\n\n hastToPath avec fichier main.c = %s \n",hashToPath(sha256file("main.c")));
    blobFile("test_ltof.txt");
    //List* l2 = ftol("test_ltof.txt");
    //printf("%s\n",ltos(l));
    //printf("ici c'est l2 %s\n",ltos(l));

	//printf("%s\n",ctos(listGet(l,1)));
    //List* m = listdir(".");
    //printf("%s\n",ltos(m)); 
	//printf("%s\n",searchList(l,"ahh"));


    printf("--------------------------------------------\n");
    List* m = listdir(".");
    printf("%s\n",ltos(m)); 
    printf("%d \n",file_exists("exo2.c"));
	return 0; 
}
