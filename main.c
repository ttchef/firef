
#include <stdio.h> 

#define FIREF_IMPL
#include "firef.h"

int main() {

    fr_Obj cube;
    fr_loadObj("cube.obj", &cube);


    fr_freeObj(&cube);

    return 0;
}


