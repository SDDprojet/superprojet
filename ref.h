#ifndef ref_h
#define ref_H

void initRefs();

void createUpdateRef(char* ref name, char* hash);

void deleteRef(char* ref name);

char* getRef(char* ref name) ;


#endif