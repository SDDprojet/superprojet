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
	
	Cell* c1 = buildCell("test");
	Cell* c2 = buildCell("ahh");
	Cell* c3 = buildCell("HZHE");
	
	insertFirst(l,c1);
	insertFirst(l,c2);
	insertFirst(l,c3);
    
    printf("%s\n",ltos(l));
	//printf("%s\n",ctos(listGet(l,1)));
    List* m = listdir(".");
    printf("%s\n",ltos(m)); 
	printf("%s\n",searchList(l,"ahh"));


    printf("--------------------------------------------\n");
    printf("%d \n",file_exists("Makefile"));
	return 0; 
}
