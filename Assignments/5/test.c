#include <stdio.h>
#include <stdlib.h>

#define ERR(l,r,label) {l=r;goto label;}

int main(int argc, char **argv) {
    int retval, in = atoi(argv[1]);
    if (in == 1)
        ERR(retval, 0, label);
    retval = 1;


label:
    return retval;
}