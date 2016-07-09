#include "utilitaire.h"

char* strMultiCat(int maxi, int num, ...) {
    va_list valist;
    va_start(valist, num);

    char* chaine = (char*) malloc(sizeof(char)*maxi);
    chaine[0] = '\0';
    unsigned int i = 0;
    for(i=0; i<num; i++) {
        strcat(chaine, va_arg(valist, char*));
    }
    va_end(valist);

    return chaine;
}

char* itoa10(int nb, unsigned int length = 200) {
    char* str = (char*) malloc(sizeof(char)*length);
    _itoa(nb, str, 10);
    return str;
}

void debugInt(int nb) {
    char strNb[20] = "";
    _itoa(nb, strNb, 10);
    MessageBox(NULL, strNb, "Debug INT", MB_OK);
}
