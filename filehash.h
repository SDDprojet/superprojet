#ifndef filehash_H
#define filehash_H


int hashFile(char* source, char* dest);
// calcule le hash du contenu du premier fichier et l’ecrit dans le deuxieme fichier

char* sha256file(char* file);
// retourne chaine de caracteres contenant le hash du fichier donne en parametre

#endif