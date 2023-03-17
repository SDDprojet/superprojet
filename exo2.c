#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

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

void insertFirst(List *L,Cell* C){
    C->next=*L;
    *L=C;
    
}

char* ctos(Cell* c){
    return c->data;
}

char* ltos(List* l){
    char* res = ctos(*l);
	while((*l)->next){
		*l=(*l)->next;
		sprintf(res,"%s|%s",res,ctos(*l));
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

Cell* searchList(List* l, char* str){
    Cell* c = (*l);
    while(c){
        
        if(strcmp(c->data,str) == 0){
            return c;
        }
        c = c->next;
        printf("%p \n",c);
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
List* stol4(char* s){
    List * L = initList();
    int debut = 0, i=0, cpt = 0;
    char *tmp = malloc(256);
    while(s[i]!= '\0'){
        printf("la valeur de i = %d \n",i);
        printf("la valeur de s = %d \n",strlen(s));
        if(s[i]!='|'){
            cpt++;
        }else if (i<strlen(s)){
            printf("on est dans la boule\n");
            tmp = strncpy(tmp, s+debut,cpt);
            Cell * C = buildCell(tmp);
            insertFirst(L,C);

            debut = i+1;
            cpt = 0;
        }
        i++;
    }
    char* last_tmp = malloc(256);
    last_tmp = strcpy(last_tmp, s+debut);
    Cell * C = buildCell(last_tmp);
    insertFirst(L,C);
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

int main(){
	List* l = initList();
	printf("%d\n","hahaha");
    printf("%c\n",97);
    printf("%d\n",strlen("1"));
	Cell* c1 = buildCell("test");
	Cell* c2 = buildCell("ahh");
	Cell* c3 = buildCell("HZHE");
	
	insertFirst(l,c1);
	insertFirst(l,c2);
	insertFirst(l,c3);
    List* l3 = stol4("cest|bon|j|abondonne");
    printf("%s\n",ltos(l3));
    printf("%s\n",ltos(l));
	//printf("%s\n",ctos(listGet(l,1)));
    //List* m = listdir(".");
    //printf("%s\n",ltos(m)); 
	//printf("%s\n",searchList(l,"ahh"));


    printf("--------------------------------------------\n");
    //printf("%d \n",file_exists("Makefile"));
	return 0; 
}
