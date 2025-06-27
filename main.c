
#include <stdio.h> 

#define FIREF_IMPL
#include "firef.h"

int main() {

    fr_ObjObject cube;
    fr_loadObjObject("cube.obj", &cube);


    fr_freeObjObject(&cube);

    return 0;
}


