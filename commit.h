#ifndef commit_H
#define commit_H

typedef struct key_value_pair {
 char* key ;
 char* value ;
 }kvp ;

 typedef struct hash_table {
 kvp** T ;
 int n ;
 int size ;
 }HashTable;

 typedef HashTable Commit ;

kvp* createKeyVal(char* key,char* val);

void freeKeyVal(kvp* kv);

char* kvts(kvp* k);

kvp* stkv(char* s);

void freeCommit(Commit* c);

Commit* initCommit();

unsigned long hash(unsigned char *str);

void commitSet(Commit* c,char* key, char* value);


Commit* createCommit(char* hash);

char* commitGet(Commit* c,char* key);

char* cts(Commit* c);

void ctf(Commit* c, char* file);

Commit *ftc(char *file);

Commit* stc(char *ch);

char* blobCommit( Commit* c );

char* commitPath(char* hash);


#endif