
#include <stdio.h> 

#define FIREF_IMPL
#include "firef.h"

int main() {

    fr_Obj cube;
    fr_loadObj("cube.obj", &cube);

    // for debugging
    fr_printObj(&cube);

    //fr_ArrayFloat test = fr_mergeVerticesNormals(&cube);


    fr_freeObj(&cube);

    return 0;
}


